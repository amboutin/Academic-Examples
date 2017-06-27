#!/usr/bin/python
#coding: utf-8

'''
Name: Aaron Boutin      Assignment: Project 2           Date: 10/MAR/2017

References/Sources: 
    CS372 Lectures and Python Software Foundation (PSF) for general setup. Stackoverflow and PSF
    as well as PythonCentral for syntax/semantic questions.

This Program uses three functions: initiateContact(), makeRequest(), and receiveFile(). 
The Program can make two types of requests, directory and file. The only allowable file format is "txt"
The arguments for each are as follows:
Directory: ftclient remoteHostName serverPort command dataPort
	where command is either "-l" for directory list or "-g" for file request. dataPort is where the client will open a socket and wait for server connection   
File Transfer: ftclient remoteHostName serverPort command fileName dataPort, where the additionaly fileName is the requested file with "txt" type appended

EXTRA CREDIT: I have the progam use threading to implement concurrent send/recieve between client and host
'''
import os
import socket
import sys
import SocketServer

'''
initiateContact()

Function initiates setup socket for connection to host, then connects and verifies the
the correct host has been connected to.

RETURNS the socket that is used in the connection

'''
def initiateContact():
        # Begin Connection
		HOST, PORT = sys.argv[1], int(sys.argv[2])					# Gets Port and Host from passed in arguments to program call
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)	# Setup TCP socket connection to server
		sock.connect((HOST, PORT))									# Attempts connection to server
		sock.send("c")												# If successful, sends verifying character to ensure correct server is connected
		received = sock.recv(1024)									# Receive verifying value from server
		if received != "C":											# If value is not received, socket is closed and program exits
			print "Incompatible Server"
			sock.close()											# Close control socket with server
			sys.exit()									
		return sock

'''
makeRequest()

Function combines program arguments into a specifically arranged string that is designed to be parsed by server
to get correct arguments for server-side processing. 

TAKES as argument the socket a request is to be made on.

'''
def makeRequest(sock):
		# Create request data from program call arguments
		sequence = [str(len(sys.argv)), socket.gethostname()]						# the variable sequence is used to concatenate the necessary parts,
		sequence.extend(sys.argv[3:])												# needed to send the server the correct request
		data = " ".join(sequence)													# The sequence list is turned into a string of values to be sent
		sock.send(data)																# The request is sent
		if sys.argv[3] == "-g":														# If it is a file request, we wait for a response from server,
			received = sock.recv(1024)												# of which it is a value that indicates whether or not a file exists.
			if received == "e":
				print sys.argv[1] + ":" + sys.argv[2] + " says FILE NOT FOUND"
				sock.close()														# Close control socket with server
				sys.exit()

'''
receiveFile()

Function uses program command argument to determine if it is to receive the directory listing or the text file.
This function checks if a file exists, and if so appends "_COPY_#" to the end, where # is an incrementing value based on
number of copies.

TAKES the socket connection used in receiving the data (data port)

'''			
def receiveFile(conn):
		if sys.argv[3] == "-l":																	# This route deals with directory structures			
			print "Receiving Directory structure from " + sys.argv[1] + ":" + sys.argv[4]		
			while(1):																			# Loop ensures all lines of data are received
				data = conn.recv(1024).strip()
				if not data: break
				words = data.split()
				for word in words:
					print(word)
					sys.stdout.flush()
			print "Directory Transfer Complete"					
		if sys.argv[3] == "-g":																	# This route deals with file transfers
			print "Receiving file "+ sys.argv[4] +" from " + sys.argv[1] + ":" + sys.argv[5]
			fileName = ogFileName = sys.argv[4]
			copyNum = 0
			while (os.path.isfile(fileName)):													# Check if file exists, if so we go into loop that deals,
				copyNum += 1																	# with duplicates. This loop increments a value and appends,
				copyString = "_COPY_" + str(copyNum)											# a copy indicator until that filename is not taken
				fileName = ogFileName[:-4]														
				fileName += copyString + ".txt"													
			f = open(fileName,"w") #															# Open file for writing text data
			while(1):																			# Loop ensures we receive all lines until no lines are sent
				data = conn.recv(1024).strip()
				if not data: break
				f.write(data)
				sys.stdout.flush()
			f.close()																			# When complete the file is closed
			print "File Transfer Complete"


'''
MAIN



'''	
if __name__ == "__main__":
	'''
	This first section deals with verifying command line arguments
	'''
	if sys.argv[3] == "-g":
		filename = sys.argv[4]
		if filename[-4:] != ".txt":												# This only allows files ending in ".txt"
			print "Incorrect Filetype, this program only accepts 'txt' files"
			sys.exit()
	if len(sys.argv) == 5:														# The number of arguments lets the program know where to check for port numbers to validate
		portConnect = sys.argv[2]
		if portConnect.isdigit() != True:
			print "Invalid Port input, must be a numeric value" 
			sys.exit()
		dataPortConnect = int(sys.argv[4])										# This variable holds the data port for receiving at

	if len(sys.argv) == 6:														# The number of arguments lets the program know where to check for port numbers to validate
		portConnect = sys.argv[2]
		if portConnect.isdigit() != True:
			print "Invalid Port input, must be a numeric value" 
			sys.exit()
		dataPortConnect = int(sys.argv[5])										# This variable holds the data port for receiving at
	'''
	This second sections uses two function to contact and request with server
	'''
	sock = initiateContact()													# Call function to contact server and verify
	makeRequest(sock)															# Send server request function

	'''
	This third section creates a socket to serve on, which the server will connect to
	for data transfer.
	'''
	print "Preparing for Data Connection"
	HOST = "localhost"
	PORT = dataPortConnect 
	s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	try:
		s.bind((HOST, PORT))
	except socket.error as msg:
		print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
		sys.exit()
	s.listen(1)
	conn, addr = s.accept()
	print 'Connected with ' + addr[0] + ':' + str(addr[1])

	receiveFile(conn)																# This function receives either the directory or text file

	sock.close()																	# Close control socket with server