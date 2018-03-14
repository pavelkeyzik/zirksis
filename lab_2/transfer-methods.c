void recv_all(int socket, void *buffer, int numBytes)
{
	int ret;               // Return value for 'recv'
	int receivedBytes;     // Total number of bytes received

	// Retrieve the given number of bytes.
	receivedBytes = 0;
	while (receivedBytes < numBytes) {
		ret = recv(socket, buffer + receivedBytes, numBytes - receivedBytes, 0);

		// Error encountered.
		if (ret == -1) {
			perror("recv");
			exit(1);
		}

		// Data received.
		else {
			receivedBytes += ret;
		}
	}
}

void send_all(int socket, void *buffer, int numBytes)
{
	int ret;           // Return value for 'send'
	int sentBytes;     // Total number of bytes sent

	// Send the given number of bytes.
	sentBytes = 0;
	while (sentBytes < numBytes) {
		ret = send(socket, buffer + sentBytes, numBytes - sentBytes, 0);

		// Error encountered.
		if (ret == -1) {
			perror("send");
			exit(1);
		}

		// Data sent.
		else {
			sentBytes += ret;
		}
	}
}