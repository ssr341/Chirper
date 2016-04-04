<!doctype html>
<html>
	<head>
		<title>Your Nest</title>
		<meta charset = "UTF-8">
	</head>
	<body>
		<?php
		session_start();
		echo '<a href = "homepage.php">To your NewsTree</a>';
		echo '<span style="float:right"><a href = "follow.php">View your Nest Fellows</a></span>';
		echo "<center><h1>";
		echo $_SESSION["username"];
		echo "'s Nest</h1></center>";
		
		echo "<h3>Here's your nest! You can chirp to your followers and view your previous chirps!</h3>";
		
		echo '<form action = "post.php" method = "post">';
		echo "Chirp below:<br/>";
		echo '<textarea name = "post" cols = "60" rows = "2" maxlength = "100"></textarea><br/>';
		echo '<input type = "submit" value = "Chirp!">';
		echo "</form> <br/><br/>";

		$user = $_SESSION["username"];
		
		$personalPosts = "personalPosts:User-" . $user . PHP_EOL;
		$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
		if (!$fp) {
			$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
			if (!$fp){
				echo $errstr;
				exit(1);
			}
		}
		fwrite($fp, $personalPosts);
		
		while($line = fgets($fp)) {
			$line = trim($line);
			$posts = explode(":", $line);
			echo "<table>";
			echo "<tr><td><b>";
			echo "Your Posts:";
			echo "</b></td></tr>";	
			// go through posts of user's friend
			for ($i = 1; $i < count($posts); $i++){
				echo "<tr><td>";
				echo $posts[$i];
				echo "</td></tr>";
			}
			echo "</table><br/>";
		}

		fclose($fp);
		
		// $file = fopen("posts.txt", "r") or die("Couldn't open post file");
		// // show user's posts
		// while( $line = fgets($file) ) {
			// $line = trim($line);
			// $posts = explode(":", $line);
			// $postsFound = false;
			// if ($posts[0] == $_SESSION["username"]){
				// echo "<table>";
				// echo "<tr><td><b>Your Posts:</b></td></tr>";
				// for ($i = 1; $i < count($posts); $i++){
					// echo "<tr><td>";
					// echo $posts[$i];
					// echo "</td></tr>";
				// }
				// echo "</table><br/><br/>";
				// $postsFound = true;
			// }
			// if ($postsFound)
				// break;
		// }
		// fclose($file);
							
		echo '<a href = "logout.php">Logout</a>';
		?>
	</body>
</html>
