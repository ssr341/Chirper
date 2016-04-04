<!doctype html>
<html>
	<head>
		<title>Followers/Following</title>
		<meta charset = "UTF-8">
	</head>
	<body> 
		<?php
		session_start();
		echo '<a href = "homepage.php">To your NewsTree</a>';
		echo '<span style="float:right"><a href = "profile.php">To your Nest</a></span>';
		
		echo "<center><h1>Nest Fellows</h1></center>";
		
		echo "<p>Want to follow someone? Enter in their username below:</p>";
		echo '<form action = "followCheck.php" method = "post">';
		echo '<input type = "text" name = "follow"><br/>';
		echo '<input type = "submit" value = "Follow!">';
		echo "</form><br/><br/>";
		
		$username = $_SESSION["username"];
			
		$follows = "currentlyFollows:User-" . $username . PHP_EOL;
		
		$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
		if (!$fp) {
			$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
			if (!$fp){
				echo $errstr;
				exit(1);
			}
		}
		fwrite($fp, $follows);
			
		echo "<table>";
		echo "<tr><td><b>Following:</b></td></tr>";	
		while($line = fgets($fp)) {
			$line = trim($line);
			$following = explode(":", $line);
			// go through posts of user's friend
			for ($i = 1; $i < count($following); $i++){
				echo "<tr><td>";
				echo $following[$i];
				echo "</td></tr>";
			}			
		}
		echo "</table><br/>";

		fclose($fp);
		
		// echo "<table><tr><td><b>Following:</b></tr></td>";
		// $file = fopen("friends.txt", "r") or die("Couldn't open friend file");
		// // search for user's friends
		// while ( $line = fgets($file) ) {
			// $line = trim($line);
			// $friends = explode(":", $line);
			// $userFound = false;
			// if ($friends[0] == $_SESSION["username"]){
				// for ($i = 1; $i < count($friends); $i++){
					// echo "<tr><td>";
					// echo $friends[$i];
					// echo "</td></tr>";
				// }
				// $userFound = true;
			// }
			// if ($userFound)
				// break;
		// }
		// echo "</table>";
		// fclose($file);
		
		echo "<br/><br/><p>Want to stop following someone? Enter in their username below:</p>";
		echo '<form action = "followRemove.php" method = "post">';
		echo '<input type = "text" name = "remove"><br/>';
		echo '<input type = "submit" value = "Remove!">';
		echo "</form><br/><br/>";
		
		echo "</table><br/>";
		echo '<a href = "logout.php">Logout</a>';
		?>
	</body>
</html>
