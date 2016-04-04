/*
From Stevens Unix Network Programming, vol 1.
Minor modifications by John Sterling
*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>       // perror, snprintf
#include <stdlib.h>      // exit
#include <unistd.h>      // close, write
#include <fstream>
#include <vector>
#include <algorithm>
#include <string.h>      // strlen
#include <string>
#include <thread>
#include <mutex>
#include <strings.h>     // bzero
#include <time.h>        // time, ctime
#include <sys/socket.h>  // socket, AF_INET, SOCK_STREAM,
// bind, listen, accept
#include <netinet/in.h>  // servaddr, INADDR_ANY, htons
using namespace std;

#define    MAXLINE        4096    // max text line length
#define    BUFFSIZE    8192    // buffer size for reads and writes
#define  SA struct sockaddr
#define    LISTENQ        1024    // 2nd argument to listen()
#define PORT_NUM    54321

mutex userMutex;
mutex postMutex;
mutex friendMutex;

mutex repUserMutex;
mutex repPostMutex;
mutex repFriendMutex;

void serverAction(int connfd);

int main(int argc, char **argv) {
	int            listenfd, connfd;  // Unix file descriptors
	struct sockaddr_in    servaddr;          // Note C use of struct
	char        buff[MAXLINE];
	time_t        ticks;

	// 1. Create the socket
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Unable to create a socket");
		exit(1);
	}

	// 2. Set up the sockaddr_in

	// zero it.
	// bzero(&servaddr, sizeof(servaddr)); // Note bzero is "deprecated".  Sigh.
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET; // Specify the family
	// use any network card present
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT_NUM);    // daytime server

	// 3. "Bind" that address object to our listening file descriptor
	if (bind(listenfd, (SA *)&servaddr, sizeof(servaddr)) == -1) {
		perror("Unable to bind port");
		exit(2);
	}

	// 4. Tell the system that we are going to use this sockect for
	//    listening and request a queue length
	if (listen(listenfd, LISTENQ) == -1) {
		perror("Unable to listen");
		exit(3);
	}


	for (;;) {
		// 5. Block until someone connects.
		//    We could provide a sockaddr if we wanted to know details of whom
		//    we are talking to.
		//    Last arg is where to put the size of the sockaddr if
		//    we asked for one
		fprintf(stderr, "Ready to connect.\n");
		if ((connfd = accept(listenfd, (SA *)NULL, NULL)) == -1) {
			perror("accept failed");
			exit(4);
		}
		fprintf(stderr, "Connected\n");

		thread serverThread([&]{serverAction(connfd); });
		serverThread.join();

		// 6. Close the connection with the current client and go back
		//    for another.
		close(connfd);
	}
}

void serverAction(int connfd){
	char directions[MAXLINE];
	read(connfd, directions, MAXLINE);
	fprintf(stderr, "%s \n", directions);
	char* login = strstr(directions, "login:");
	char* viewPosts = strstr(directions, "viewPosts:");
	char* personalPosts = strstr(directions, "personalPosts:");
	char* postIt = strstr(directions, "postIt:");
	char* reg = strstr(directions, "register:");
	char* friends = strstr(directions, "currentlyFollows:");
	char* followReq = strstr(directions, "wantToFollow:");
	char* unfollow = strstr(directions, "unfollow:");

	// check to see if files exist
	userMutex.lock();
	friendMutex.lock();
	postMutex.lock();
	ifstream userCheck("server2/users.txt");
	ifstream friendCheck("server2/friends.txt");
	ifstream postCheck("server2/posts.txt");

	// if any don't exist, get the replicate file
	if (!userCheck){
		repUserMutex.lock();
		userCheck.open("server1/users.txt");
		if (!userCheck)
			perror("User file missing!");
		ofstream temp("server2/users.txt");
		string line;
		while (getline(userCheck, line))
			temp << line << "\n";
		temp.close();
		repUserMutex.unlock();
	}

	if (!friendCheck){
		repFriendMutex.lock();
		friendCheck.open("server1/friends.txt");
		if (!friendCheck)
			perror("Friend file missing!");
		ofstream temp("server2/friends.txt");
		string line;
		while (getline(friendCheck, line))
			temp << line << "\n";
		temp.close();
		repFriendMutex.unlock();
	}

	if (!postCheck){
		repPostMutex.lock();
		postCheck.open("server1/posts.txt");
		if (!postCheck)
			perror("Post file missing!");
		ofstream temp("server2/posts.txt");
		string line;
		while (getline(postCheck, line))
			temp << line << "\n";
		temp.close();
		repPostMutex.unlock();
	}

	userCheck.close();
	friendCheck.close();
	postCheck.close();
	userMutex.unlock();
	friendMutex.unlock();
	postMutex.unlock();

	// check to see if username and password match
	if (login != NULL){
		lock_guard<mutex> lock(userMutex);

		// check to see if user file exists before parsing
		ifstream file("server2/users.txt");
		if (!file){
			perror("Connection failed, file does not exist");
		}
		else{
			char username[30] = " ";
			char password[20] = " ";
			login = login + 6;
			int count = 0;

			// get username from the input
			while (count < 30 && *login != ':'){
				username[count] = *login;
				login = login + 1;
				count++;
			}
			username[strlen(username)] = '\0';
			count = 0;
			login = login + 1;

			// get password from the input
			while (count < 20 && *login != '\n'){
				password[count] = *login;
				login = login + 1;
				count++;
			}
			password[strlen(password)] = '\0';

			// look in user file for username and password
			string line;
			bool found = false;
			while (getline(file, line) && !found){
				char charLine[200];
				strcpy(charLine, line.c_str());
				char* user = strstr(charLine, username);
				char* pass = strstr(charLine, password);
				if (user == NULL || pass == NULL){
					string error = "Wrong username or password.\n";
					write(connfd, error.c_str(), error.length());
				}
				else{
					string success = "Success!\n";
					write(connfd, success.c_str(), success.length());
					found = true;
				}
			}
		}
		file.close();

	}

	if (viewPosts != NULL){
		unique_lock<mutex> friendLock(friendMutex, defer_lock);
		unique_lock<mutex> postLock(postMutex, defer_lock);
		lock(friendMutex, postMutex);

		ifstream fFile("server2/friends.txt");
		ifstream pFile("server2/posts.txt");
		if (!fFile && !pFile)
			perror("Connection failed, file does not exist");
		else{
			fprintf(stderr, "viewing post\n");
			fprintf(stderr, "viewing post: %s\n", viewPosts);
			char username[30] = " ";
			int count = 0;
			viewPosts = viewPosts + 10;
			fprintf(stderr, "viewing post: %s\n", viewPosts);
			while (count < 20 && *viewPosts != '\n'){
				username[count] = *viewPosts;
				viewPosts = viewPosts + 1;
				count++;
			}
			fprintf(stderr, "username: %s\n", username);

			// search for user's friends
			vector<string> friends;
			string line;
			bool found = false;
			while (getline(fFile, line) && !found){
				char findFriendList[100] = "User-";
				strcat(findFriendList, username);
				size_t userFound = line.find(findFriendList);
				if (userFound != string::npos){
					fprintf(stderr, "user found\n");
					size_t start = 0;
					size_t end = 0;
					size_t colon = line.find(":");

					// colon means friend exists
					if (colon != string::npos){
						end = line.find(":");
						while (end != std::string::npos)
						{
							char friendc[200];
							string friendss = line.substr(start, end - start);
							strcpy(friendc, friendss.c_str());
							fprintf(stderr, "friend: %s\n", friendc);
							friends.push_back(line.substr(start, end - start));
							start = end + 1;
							end = line.find(":", start);
						}
						char friendc[200];
						string lastFriend = line.substr(start, line.length());
						strcpy(friendc, lastFriend.c_str());
						fprintf(stderr, "friend: %s\n", friendc);
						friends.push_back(line.substr(start, line.length()));
					}
					found = true;
				}
			}

			// search for post of friends
			while (getline(pFile, line)){
				char postsLine[200];
				strcpy(postsLine, line.c_str());
				fprintf(stderr, "line: %s\n", postsLine);
				bool friendFound = false;
				for (int i = 1; i < friends.size(); i++){
					string findFriend = "User-";
					findFriend.append(friends[i]);
					findFriend.append(":");
					char friendi[200] = " ";
					strcpy(friendi, findFriend.c_str());
					fprintf(stderr, "friend: %s\n", friendi);
					if (line.find(findFriend) != string::npos){
						friendFound = true;
						fprintf(stderr, "friend Found\n");
					}
				}

				// if friend is on friend's list, copy their posts
				if (friendFound){
					string output = line;
					output.append("\n");
					write(connfd, output.c_str(), output.length());
				}
			}
		}
		friendMutex.unlock();
		postMutex.unlock();

		fFile.close();
		pFile.close();
	}

	if (personalPosts != NULL){
		lock_guard<mutex> lock(postMutex);

		ifstream file("server2/posts.txt");
		if (!file)
			perror("Connection failed, file does not exist");
		else{
			char username[30] = " ";
			int count = 0;
			personalPosts = personalPosts + 14;
			while (count < 20 && *personalPosts != '\n'){
				username[count] = *personalPosts;
				personalPosts = personalPosts + 1;
				count++;
			}
			fprintf(stderr, "username: %s\n", username);

			// search for user's posts
			string line;
			bool found = false;
			while (getline(file, line) && !found){
				size_t userFound = line.find(username);
				if (userFound != string::npos){
					string output = line;
					output.append("\n");
					char outputtest[500];
					strcpy(outputtest, output.c_str());
					fprintf(stderr, "outputting: %s\n", outputtest);
					write(connfd, output.c_str(), output.length());
					found = true;
				}
			}
		}

		file.close();
	}

	if (postIt != NULL){
		lock_guard<mutex> lock(postMutex);
		lock_guard<mutex> lock2(repPostMutex);

		ifstream pFile("server2/posts.txt");
		if (!pFile)
			perror("Connection failed, file does not exist");
		else{
			ofstream temp("server2/temp.txt");
			// get username
			char username[30] = " ";
			int count = 0;
			postIt = postIt + 7;
			while (count < 20 && *postIt != ':'){
				username[count] = *postIt;
				postIt = postIt + 1;
				count++;
			}

			// get post
			postIt = postIt + 1;
			char post[200] = " ";
			count = 0;
			while (count < 200 && *postIt != '\n'){
				post[count] = *postIt;
				postIt = postIt + 1;
				count++;
			}
			fprintf(stderr, "username: %s. post: %s\n", username, post);

			// copy entire file but user post line
			string line;
			string userLine = " ";
			while (getline(pFile, line)){
				char cline[1000];
				strcpy(cline, line.c_str());
				fprintf(stderr, "%s\n", cline);
				size_t userFound = line.find(username);
				if (userFound != string::npos){
					fprintf(stderr, "userfound.\n");
					userLine = line;
				}
				else
					temp << line << "\n";
			}

			// delete old file and replace with new
			temp.close();
			pFile.close();
			remove("server2/posts.txt");
			rename("server2/temp.txt", "server2/posts.txt");

			// add editted line
			userLine.append(post);
			userLine.append(":");

			temp.open("server2/posts.txt", ios_base::app);
			temp << userLine << "\n";
			temp.close();
			string output = "Success!\n";
			write(connfd, output.c_str(), output.length());

			// add to replicated file. delete only line that pertains and make new file containing everything else
			pFile.open("server1/posts.txt");
			temp.open("server1/temp.txt");
			while (getline(pFile, line)){
				char cline[1000];
				strcpy(cline, line.c_str());
				fprintf(stderr, "%s\n", cline);
				size_t userFound = line.find(username);
				if (userFound != string::npos){
					fprintf(stderr, "userfound.\n");
				}
				else
					temp << line << "\n";
			}

			// delete old file and replace with new
			temp.close();
			pFile.close();
			remove("server1/posts.txt");
			rename("server1/temp.txt", "server1/posts.txt");

			temp.open("server1/posts.txt", ios_base::app);
			temp << userLine << "\n";
			temp.close();
			output = "Replicate Success!\n";
			write(connfd, output.c_str(), output.length());

		}
	}

	if (reg != NULL){
		unique_lock<mutex> userLock(userMutex, defer_lock);
		unique_lock<mutex> postLock(postMutex, defer_lock);
		unique_lock<mutex> friendLock(friendMutex, defer_lock);
		lock(userLock, postLock, friendLock);

		unique_lock<mutex> userLock2(repUserMutex, defer_lock);
		unique_lock<mutex> postLock2(repPostMutex, defer_lock);
		unique_lock<mutex> friendLock2(repFriendMutex, defer_lock);
		lock(userLock2, postLock2, friendLock2);

		// check to see if user file exists before parsing
		fstream file("server2/users.txt");
		if (!file)
			perror("Connection failed, file does not exist");
		else{
			char username[30] = " ";
			char password[20] = " ";
			reg = reg + 9;
			int count = 0;

			// get username from the input
			while (count < 30 && *reg != ':'){
				username[count] = *reg;
				reg = reg + 1;
				count++;
			}
			username[strlen(username)] = '\0';
			count = 0;
			reg = reg + 1;

			// get password from the input
			while (count < 20 && *reg != '\n'){
				password[count] = *reg;
				reg = reg + 1;
				count++;
			}
			password[strlen(password)] = '\0';

			// look in user file for username
			string line;
			bool found = false;
			while (getline(file, line)){
				char cline[1000];
				strcpy(cline, line.c_str());
				fprintf(stderr, "%s\n", cline);
				size_t userFound = line.find(username);
				if (userFound != string::npos){
					fprintf(stderr, "userfound.\n");
					found = true;
				}
			}

			file.close();

			// if username doesn't exist, add it
			if (!found){
				string userLine = username;
				userLine.append(":");
				userLine.append(password);
				char cline[1000] = " ";
				strcpy(cline, userLine.c_str());
				fprintf(stderr, "%s\n", cline);
				ofstream ofile;
				ofile.open("server2/users.txt", ios_base::app);
				if (ofile)
					fprintf(stderr, "opened user\n");
				ofile << userLine << "\n";
				ofile.close();
				fprintf(stderr, "wrote to users.\n");

				// add to replicate
				ofile.open("server1/users.txt", ios_base::app);
				ofile << userLine << "\n";
				ofile.close();
				fprintf(stderr, "wrote to replicate users.\n");

				// add user to friend and post files also
				userLine = username;
				userLine.append(":");
				strcpy(cline, userLine.c_str());
				fprintf(stderr, "%s\n", cline);
				ofile.open("server2/posts.txt", ios_base::app);
				if (ofile)
					fprintf(stderr, "opened posts\n");
				ofile << userLine << "\n";
				ofile.close();
				fprintf(stderr, "wrote to posts.\n");

				// replicate posts
				ofile.open("server1/posts.txt", ios_base::app);
				ofile << userLine << "\n";
				ofile.close();
				fprintf(stderr, "wrote to replicate posts.\n");

				ofile.open("server2/friends.txt", ios_base::app);
				if (ofile)
					fprintf(stderr, "opened friends\n");
				ofile << userLine << "\n";
				ofile.close();
				fprintf(stderr, "wrote to friends.\n");

				ofile.open("server1/friends.txt", ios_base::app);
				ofile << userLine << "\n";
				ofile.close();
				fprintf(stderr, "wrote to replicate friends.\n");

				string success = "Success!\n";
				write(connfd, success.c_str(), success.length());
			}
			else{
				string output = "Username exists.\n";
				write(connfd, output.c_str(), output.length());
			}
		}
		userMutex.unlock();
		friendMutex.unlock();
		postMutex.unlock();

		repUserMutex.unlock();
		friendMutex.unlock();
		postMutex.unlock();
	}

	if (friends != NULL){
		lock_guard<mutex> lock(friendMutex);

		ifstream file("server2/friends.txt");
		if (!file)
			perror("Connection failed, file does not exist");
		else{
			char username[30] = " ";
			int count = 0;
			friends = friends + 17;
			while (count < 30 && *friends != '\n'){
				username[count] = *friends;
				friends = friends + 1;
				count++;
			}
			fprintf(stderr, "username: %s\n", username);

			// search for user's friends
			string line;
			bool found = false;
			while (getline(file, line) && !found){
				size_t userFound = line.find(username);
				if (userFound != string::npos){
					string output = line;
					output.append("\n");
					char outputtest[500];
					strcpy(outputtest, output.c_str());
					fprintf(stderr, "outputting: %s\n", outputtest);
					write(connfd, output.c_str(), output.length());
					found = true;
				}
			}
			file.close();
		}
	}

	if (followReq != NULL){
		unique_lock<mutex> userLock(userMutex, defer_lock);
		unique_lock<mutex> friendLock(friendMutex, defer_lock);
		lock(userMutex, friendMutex);

		unique_lock<mutex> userLock2(repUserMutex, defer_lock);
		unique_lock<mutex> friendLock2(repFriendMutex, defer_lock);
		lock(repUserMutex, repFriendMutex);

		ifstream fFile("server2/friends.txt");
		ifstream uFile("server2/users.txt");
		if (!fFile || !uFile)
			perror("Connection failed, file does not exist");
		else{
			// get username
			char username[30] = " ";
			int count = 0;
			followReq = followReq + 13;
			while (count < 30 && *followReq != ':'){
				username[count] = *followReq;
				followReq = followReq + 1;
				count++;
			}

			// get friend
			followReq = followReq + 1;
			char addFriend[30] = " ";
			count = 0;
			while (count < 30 && *followReq != '\n'){
				addFriend[count] = *followReq;
				followReq = followReq + 1;
				count++;
			}
			fprintf(stderr, "username: %s. friend: %s\n", username, addFriend);

			// check if friend exists
			string users;
			bool exists = false;
			string friendUser = "User-";
			friendUser.append(addFriend);
			while (getline(uFile, users)){
				if (users.find(friendUser) != string::npos){
					fprintf(stderr, "friend exists.\n");
					exists = true;
				}
			}
			uFile.close();

			// check to see if user is on friends list already. if not, copy all lines but user's line, add friend to it and append to
			// new file
			if (exists){
				string check;
				bool friendsAlready = false;
				while (getline(fFile, check)){
					char cline[1000];
					strcpy(cline, check.c_str());
					fprintf(stderr, "%s\n", cline);
					size_t userFound = check.find(username);
					if (userFound != string::npos){
						fprintf(stderr, "userfound.\n");
						size_t friendFound = check.find(addFriend);
						if (friendFound != string::npos){
							fprintf(stderr, "already friends.\n");
							friendsAlready = true;
						}
					}
				}
				fFile.close();

				// not friends with person you wish to add
				if (!friendsAlready){
					fFile.open("server2/friends.txt");
					if (fFile)
						fprintf(stderr, "opened fFile.\n");
					ofstream temp("server2/temp.txt");
					if (temp)
						fprintf(stderr, "opened temp.\n");
					string line;
					string userLine = " ";
					while (getline(fFile, line)){
						char cline[1000];
						strcpy(cline, line.c_str());
						fprintf(stderr, "%s\n", cline);
						size_t userFound = line.find(username);
						if (userFound != string::npos){
							fprintf(stderr, "userfound.\n");
							userLine = line;
						}
						else
							temp << line << "\n";
					}

					// delete old file and replace with new
					temp.close();
					fFile.close();
					remove("server2/friends.txt");
					rename("server2/temp.txt", "server2/friends.txt");

					// add editted line
					userLine.append(addFriend);
					userLine.append(":");

					temp.open("server2/friends.txt", ios_base::app);
					temp << userLine << "\n";
					temp.close();
					string output = "Success!\n";
					write(connfd, output.c_str(), output.length());

					// do same with replicate file
					fFile.open("server1/friends.txt");
					temp.open("server1/temp.txt");
					while (getline(fFile, line)){
						size_t userFound = line.find(username);
						if (userFound != string::npos){
							fprintf(stderr, "userfound.\n");
						}
						else
							temp << line << "\n";
					}

					temp.close();
					fFile.close();
					remove("server1/friends.txt");
					rename("server1/temp.txt", "server1/friends.txt");

					temp.open("server1/friends.txt", ios_base::app);
					temp << userLine << "\n";
					temp.close();
					output = "Replicate Success!\n";
					write(connfd, output.c_str(), output.length());
				}

				// already friends with user trying to add
				else{
					string output = "Already friends.\n";
					write(connfd, output.c_str(), output.length());
				}
			}

			// user trying to add doesn't exist
			else{
				fFile.close();
				string output = "Does not exist.\n";
				write(connfd, output.c_str(), output.length());
			}
		}
		userMutex.unlock();
		friendMutex.unlock();

		repUserMutex.unlock();
		repFriendMutex.unlock();
	}

	if (unfollow != NULL){
		lock_guard<mutex> lock(friendMutex);
		lock_guard<mutex> lock2(repFriendMutex);

		ifstream fFile("server2/friends.txt");
		if (!fFile)
			perror("Connection failed, file does not exist");
		else{
			// get username
			char username[30] = " ";
			int count = 0;
			unfollow = unfollow + 9;
			while (count < 30 && *unfollow != ':'){
				username[count] = *unfollow;
				unfollow = unfollow + 1;
				count++;
			}

			// get friend
			unfollow = unfollow + 1;
			char removeFriend[30] = " ";
			count = 0;
			while (count < 30 && *unfollow != '\n'){
				removeFriend[count] = *unfollow;
				unfollow = unfollow + 1;
				count++;
			}
			fprintf(stderr, "username: %s. friend: %s\n", username, removeFriend);

			// check to see if user is on friends list already. if yes, copy all lines but user's line, remove friend from it and
			// append to new file
			string check;
			bool friendsAlready = false;
			while (getline(fFile, check)){
				char cline[1000];
				strcpy(cline, check.c_str());
				fprintf(stderr, "%s\n", cline);
				size_t userFound = check.find(username);
				if (userFound != string::npos){
					fprintf(stderr, "userfound.\n");
					size_t friendFound = check.find(removeFriend);
					if (friendFound != string::npos){
						fprintf(stderr, "already friends.\n");
						friendsAlready = true;
					}
				}
			}
			fFile.close();

			// remove friend
			if (friendsAlready){
				fFile.open("server2/friends.txt");
				ofstream temp("server2/temp.txt");
				string line;
				string userLine = " ";
				while (getline(fFile, line)){
					char cline[1000];
					strcpy(cline, line.c_str());
					fprintf(stderr, "%s\n", cline);
					size_t userFound = line.find(username);
					if (userFound != string::npos){
						fprintf(stderr, "userfound.\n");
						userLine = line;
					}
					else
						temp << line << "\n";
				}

				// delete old file and replace with new
				temp.close();
				fFile.close();
				remove("server2/friends.txt");
				rename("server2/temp.txt", "server2/friends.txt");

				// add editted line
				string toRemove = removeFriend;
				toRemove.append(":");
				size_t index = userLine.find(toRemove);

				if (index != string::npos)
					userLine.erase(index, toRemove.length());

				temp.open("server2/friends.txt", ios_base::app);
				temp << userLine << "\n";
				temp.close();
				string output = "Success!\n";
				write(connfd, output.c_str(), output.length());

				// change replicate files
				fFile.open("server1/friends.txt");
				temp.open("server1/temp.txt");
				while (getline(fFile, line)){
					size_t userFound = line.find(username);
					if (userFound != string::npos){
						fprintf(stderr, "userfound.\n");
					}
					else
						temp << line << "\n";
				}

				temp.close();
				fFile.close();
				remove("server1/friends.txt");
				rename("server1/temp.txt", "server1/friends.txt");

				temp.open("server1/friends.txt", ios_base::app);
				temp << userLine << "\n";
				temp.close();
				output = "Replicate Success!\n";
				write(connfd, output.c_str(), output.length());
			}

			// cannot remove friend because not currently friends
			else{
				string output = "Not friends.\n";
				write(connfd, output.c_str(), output.length());
			}
		}
	}
}

