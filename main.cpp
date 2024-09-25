#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <functional>
#include <sstream>
#include <atomic>
#include "Graph.hpp"
#include "StrategyFactory.hpp"
#include "MSTServer.hpp"
#include "ThreadPoll.hpp"

using namespace std;

const int PORT = 9034;
atomic<bool> server_running(true);

string trim(const string &s){
    auto wsfront = find_if_not(s.begin(), s.end(), [](int c)
                                    { return isspace(c); });
    auto wsback = find_if_not(s.rbegin(), s.rend(), [](int c)
                                   { return isspace(c); })
                      .base();
    return (wsback <= wsfront ? string() : string(wsfront, wsback));
}

void send_response(int client_socket, const string &message){
    string response = message + "\n> ";
    send(client_socket, response.c_str(), response.length(), 0);
}

void show_options(int client_socket){
    send_response(client_socket, "Available commands: init, change_graph, kruskal, prim, quit, exit");
}

string graph_to_string(const Graph& graph) {
    ostringstream oss;
    oss << "Graph structure:\n";
    for (int i = 0; i < graph.getNumVertices(); ++i) {
        oss << "Vertex " << i << " -> ";
        auto edges = graph.getAdjacentEdges(i);
        for (const auto& edge : edges) {
            oss << "(" << edge.first << ", " << edge.second << ") ";
        }
        oss << "\n";
    }
    return oss.str();
}

void handle_client(int client_socket, MSTServer &server, int thread_id){
    char buffer[1024] = {0};
    int clientId = client_socket; // Use the socket as a unique client ID

    // Notify the client about which thread is serving them
    string thread_message = "You are being served by thread " + to_string(thread_id) + "\n";
    send_response(client_socket, thread_message);

    show_options(client_socket);

    while (server_running){
        memset(buffer, 0, 1024);
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0){
            cout << "Client disconnected" << endl;
            break;
        }

        string command = trim(string(buffer));
        cout << "Received command: '" << command << "'" << endl;

        if (command == "quit" || command == "exit"){
            cout << "Client requested to quit. Closing connection." << endl;
            send_response(client_socket, "Goodbye!");
            break;
        }
        else if (command == "init"){
            cout << "Initializing new graph. Waiting for vertices and edges count..." << endl;
            send_response(client_socket, "Enter number of vertices and edges:");

            memset(buffer, 0, 1024);
            valread = read(client_socket, buffer, 1024);
            if (valread <= 0){
                cout << "Client disconnected" << endl;
                break;
            }

            istringstream iss(buffer);
            int numVertices, numEdges;
            iss >> numVertices >> numEdges;

            if (numVertices <= 0 || numEdges < 0){
                send_response(client_socket, "Invalid number of vertices or edges.");
                show_options(client_socket);
                continue;
            }

            Graph current_graph(numVertices);

            cout << "Waiting for " << numEdges << " edges..." << endl;
            send_response(client_socket, "Enter edges in format: source destination weight");

            for (int i = 0; i < numEdges; ++i){
                memset(buffer, 0, 1024);
                valread = read(client_socket, buffer, 1024);
                if (valread <= 0){
                    cout << "Client disconnected" << endl;
                    return;
                }

                istringstream edge_iss(buffer);
                int u, v, weight;
                edge_iss >> u >> v >> weight;
                current_graph.addEdge(u, v, weight);
                cout << "Added edge: " << u << " - " << v << " (weight: " << weight << ")" << endl;
            }

            server.setGraph(clientId, current_graph);
            send_response(client_socket, "Graph initialized successfully. Visualizing graph...");
            server.visualizeGraph(clientId);
            show_options(client_socket);
        }

        else if (command == "change_graph"){
            cout << "Updating graph" << endl;
            if(!server.clientGraphs.count(clientId)){
                send_response(client_socket, "Please initialize a graph first using 'init' command.");
                show_options(client_socket);
                continue;
            }
            send_response(client_socket, "Enter what you want to do: add_edge, remove_edge, add_vertex, remove_vertex");
            
            memset(buffer, 0, 1024);
            valread = read(client_socket, buffer, 1024);
            if (valread <= 0){
                cout << "Client disconnected" << endl;
                break;
            }
            
            string subcommand = trim(string(buffer));
            Graph& client_graph = *(server.clientGraphs[clientId]);
            
            if(subcommand == "add_edge"){
                send_response(client_socket, "Enter the edge in format: source destination weight");
                memset(buffer, 0, 1024);
                valread = read(client_socket, buffer, 1024);
                if (valread <= 0){
                    cout << "Client disconnected" << endl;
                    break;
                }

                istringstream edge_iss(buffer);
                int u, v, weight;
                edge_iss >> u >> v >> weight;
                client_graph.addEdge(u, v, weight);
                send_response(client_socket, "Edge added successfully. Updated graph:\n" + graph_to_string(client_graph));
            }
            else if(subcommand == "remove_edge"){
                send_response(client_socket, "Enter the edge in format: source destination");
                memset(buffer, 0, 1024);
                valread = read(client_socket, buffer, 1024);
                if (valread <= 0){
                    cout << "Client disconnected" << endl;
                    break;
                }

                istringstream edge_iss(buffer);
                int u, v;
                edge_iss >> u >> v;
                client_graph.removeEdge(u, v);
                send_response(client_socket, "Edge removed successfully. Updated graph:\n" + graph_to_string(client_graph));
            }
            else if(subcommand == "add_vertex"){
                send_response(client_socket, "Enter the vertex to add");
                memset(buffer, 0, 1024);
                valread = read(client_socket, buffer, 1024);
                if (valread <= 0){
                    cout << "Client disconnected" << endl;
                    break;
                }

                istringstream vertex_iss(buffer);
                int vertex;
                vertex_iss >> vertex;
                client_graph.addVertex(vertex);
                send_response(client_socket, "Vertex added successfully. Updated graph:\n" + graph_to_string(client_graph));
            }
            else if(subcommand == "remove_vertex"){
                send_response(client_socket, "Enter the vertex to remove");
                memset(buffer, 0, 1024);
                valread = read(client_socket, buffer, 1024);
                if (valread <= 0){
                    cout << "Client disconnected" << endl;
                    break;
                }

                istringstream vertex_iss(buffer);
                int vertex;
                vertex_iss >> vertex;
                client_graph.removeVertex(vertex);
                send_response(client_socket, "Vertex removed successfully. Updated graph:\n" + graph_to_string(client_graph));
            }
            else{
                send_response(client_socket, "Invalid subcommand for change_graph.");
            }
            show_options(client_socket);
        }       

        else if (command == "kruskal" || command == "prim"){
            if (!server.clientGraphs.count(clientId)){
                send_response(client_socket, "Please initialize a graph first using 'init' command.");
                show_options(client_socket);
                continue;
            }

            cout << "Received " << command << " command. Processing..." << endl;

            server.solveMST(clientId, command);
            server.calculateMeasurements(clientId);

            ostringstream oss;
            oss << "MST Results:\n"
                << "Total weight: " << server.totalWeight[clientId] << "\n"
                << "Longest distance: " << server.longestDistance[clientId] << "\n"
                << "Average distance: " << server.averageDistance[clientId] << "\n"
                << "Shortest MST distance: " << server.shortestMSTDistance[clientId];

            send_response(client_socket, oss.str());
            cout << "Sent MST results to client" << endl;

            server.visualizeMST(clientId);
            show_options(client_socket);
        }
        else{
            send_response(client_socket, "Invalid command.");
            show_options(client_socket);
        }
    }

    close(client_socket);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <number_of_threads>" << endl;
        return 1;
    }

    int num_threads = stoi(argv[1]);
    cout << "Number of threads: " << num_threads << endl;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) { // Set to listen to only one connection at a time
        perror("listen");
        exit(EXIT_FAILURE);
    }

    MSTServer mst_server(num_threads);  // Pass num_threads to the constructor
    ThreadPoll thread_pool(num_threads);

    cout << "Server listening on port " << PORT << " with " << num_threads << " threads" << endl;

    while (server_running) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            continue;
        }

        cout << "New client connected" << endl;

        // Create a lambda function to handle the client and add it to the thread pool
        thread_pool.enqueue(new_socket, [new_socket, &mst_server](int thread_id) {
            handle_client(new_socket, mst_server, thread_id);
            cout << "Client disconnected, thread " << thread_id << " is free" << endl;
            close(new_socket);  // Close client socket when done
        });
    }

    close(server_fd);
    return 0;
}