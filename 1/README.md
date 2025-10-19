# Trivial distributed file system (DFS)

A client for some provided trivial (read-only) distributed file system.

This system uses a simple version of a URL to identify files and their locations.

File access within the system is handled via the File Service Protocol (FSP). For this project, it is sufficient to implement only one type of request - the GET command.

The system uses domain names, which are translated into IP addresses using the Name Service Protocol (NSP). This protocol allows obtaining the IP address and port number where the corresponding file server is running.
