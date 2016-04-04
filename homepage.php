<!doctype html>
<html>
	<head>
		<title>NewsTree</title>
		<meta charset = "UTF-8">
	</head>
	<body> 
		<?php 
		session_start();
		echo '<a href = "profile.php">To Your Nest</a>';
		echo '<span style = "float:right"><a href = "follow.php">View your Nest Fellows</a></span>';
		echo "<h1><center>";
		echo $_SESSION["username"];
		echo "'s NewsTree</center></h1>";
		echo "<h3>Here's your NewsTree! You can view the posts of the people you follow!</h3>";
		
		$user = $_SESSION["username"];
		
		$postInfo = "viewPosts:" . $user . PHP_EOL;
		$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
		if (!$fp) {
			$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
			if (!$fp){
				echo $errstr;
				exit(1);
			}
		}
		fwrite($fp, $postInfo);
		
		while($line = fgets($fp)) {
			$line = trim($line);
			$posts = explode(":", $line);
			echo "<table>";
			echo "<tr><td><b>";
			echo $posts[0];
			echo ":</b></td></tr>";	
			// go through posts of user's friend
			for ($i = 1; $i < count($posts); $i++){
				echo "<tr><td>";
				echo $posts[$i];
				echo "</td></tr>";
			}
			echo "</table><br/>";
		}

		fclose($fp);
		
		// $ffile = fopen("friends.txt", "r") or die("Couldn't open friend file"); // friend file
		// $pfile = fopen("posts.txt", "r") or die("Couldn't open post file"); // post file
		// // search for user's friends
		// while ( $line = fgets($ffile) ) {
			// $line = trim($line);
			// $friends = explode(":", $line);
			// $userFound = false;
			// if ($friends[0] == $_SESSION["username"]){
				// // go through user's friends and output their posts
				// for ($i = 1; $i < count($friends); $i++){
					// // find posts of friends
					// while( $postline = fgets($pfile) ) {
						// $postline = trim($postline);
						// $posts = explode(":", $postline);
						// $postsFound = false;
						// if ($posts[0] == $friends[$i]){
							// echo "<table>";
							// echo "<tr><td><b>";
							// echo $friends[$i];
							// echo ":</b></td></tr>";
							// // go through posts of user's friend
							// for ($j = 1; $j < count($posts); $j++){
								// echo "<tr><td>";
								// echo $posts[$j];
								// echo "</td></tr>";
							// }
							// echo "</table><br/>";
							// $postsFound = true;
						// }
						// if ($postsFound)
							// break;
					// }	
					// fseek($pfile, 0);
				// }
				// $userFound = true;
			// }
			// if ($userFound)
				// break;
		// }
		// fclose($pfile);
		// fclose($ffile);
		
		echo '<br/><a href = "logout.php">Logout</a>';
		?>
	</body>
</html>
