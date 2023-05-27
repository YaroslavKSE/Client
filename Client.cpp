#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib") // link with ws2_32.lib


int** fillmatrix(const int matrixsize) {
	int** matrix = new int* [matrixsize];

	// providing a seed value
	srand((unsigned)time(NULL));

	for (int i = 0; i < matrixsize; i++)
	{
		matrix[i] = new int[matrixsize];
		for (int j = 0; j < matrixsize; j++)
		{
			matrix[i][j] = rand() % 10;
		}
	}
	return matrix;
}

void printmatrix(int** matrix, int matrixsize)
{
	for (int i = 0; i < matrixsize; ++i) {
		for (int j = 0; j < matrixsize; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

int main() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "WSAStartup failed.\n";
		return 1;
	}
	// create a socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		std::cerr << "Error creating socket: " << WSAGetLastError() << "\n";
		WSACleanup();
		return 1;
	}
	// connect to server
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);
	serverAddr.sin_port = htons(8080); // replace with actual server port
	if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) ==
		SOCKET_ERROR) {
		std::cerr << "Error connecting to server: " << WSAGetLastError() << "\n";
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// send matrix size to server

	int matrixSize = 3;
	int bytesSentFirst = send(clientSocket, (char*)&matrixSize, sizeof(matrixSize), 0);
	if (bytesSentFirst == SOCKET_ERROR) {
		std::cerr << "Error sending matrix size: " << WSAGetLastError() << "\n";
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// send the matrix to server
	int** sendBuffer = fillmatrix(matrixSize); // replace with actual matrix values
	int bytesSentSecond = send(clientSocket, (char*)sendBuffer, sizeof(sendBuffer), 0);
	if (bytesSentSecond == SOCKET_ERROR) {
		std::cerr << "Error sending matrix: " << WSAGetLastError() << "\n";
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	// receive result from server
	int** recvBuffer;
	int bytesReceived = recv(clientSocket, (char*)&recvBuffer, sizeof(recvBuffer), 0);
	if (bytesReceived == SOCKET_ERROR) {
		std::cerr << "Error receiving data: " << WSAGetLastError() << "\n";
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	
	std::cout << "Result: " << recvBuffer << "\n";
	
	// cleanup
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
