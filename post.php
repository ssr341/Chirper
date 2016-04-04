<!doctype html>
<html>
	<head>
		<title>Your Nest</title>
		<meta charset = "UTF-8">
	</head>
	<body>
		<?php
		session_start();
		echo "<h1><center>";
		echo $_SESSION["username"];
		echo "'s Nest</h1></center>";
		
		$user = $_SESSION["username"];
		$chirp = $_POST["post"];
		
		$postIt = "postIt:User-" . $user . ":" . $chirp . PHP_EOL;

		$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
		if (!$fp) {
			$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
			if (!$fp){
				echo $errstr;
				exit(1);
			}
		}
		fwrite($fp, $postIt);
		
		$posted = false; 
		
		while($line = fgets($fp)) {
			if($line == "Success!\n")
				$posted = true;
		}

		   fclose($fp);
		
		// $arr = file('posts.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
		// $remove = "";
		// $file = fopen("posts.txt", "r+") or die("Couldn't open post file");
		// while ( $line = fgets($file) ){
			// $line = trim($line);
			// $posts = explode(":", $line);
			// $userFound = false;
			// if ($posts[0] == $_SESSION["username"]){
				// $remove = $line;
				// $userFound = true;
			// }
			// if ($userFound)
				// break;
		// }
		// $arr = array_filter($arr, function($item) use ($remove) {
			// return $item != $remove;
		// });
		// file_put_contents('posts.txt', implode($arr, PHP_EOL));
		// $line = PHP_EOL . $line . ":" . $chirp . PHP_EOL;
		// file_put_contents("posts.txt", $line, FILE_APPEND);
		
		
		if ($posted){
			echo 'Your message "'; 
			echo $chirp;
			echo '" has been posted.';
		}
		else {
			echo "There was an error posting your message. Please try again.";
		}
		echo "<br/><br/>You will be redirected in 3 seconds or click <a href=\"profile.php\">here</a>.";
		header("refresh: 3; profile.php");
		?>
	</body>
</html>
