<!doctype html>
<html>
	<head>
		<title>Followers/Following</title>
		<meta charset = "UTF-8">
	</head>
	<body> 
		<?php
		session_start();
		echo "<center><h1>Nest Fellows</h1></center>";
	
		$username = $_SESSION["username"];
		$fname = $_POST["follow"];
		$follow = false;
		$exist = true;
		
		$followRequest = "wantToFollow:User-" . $username . ":" . $fname . PHP_EOL;

		$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
		if (!$fp) {
			$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
			if (!$fp){
				echo $errstr;
				exit(1);
			}
		}
		fwrite($fp, $followRequest);
		
		while($line = fgets($fp)) {
			if($line == "Success!\n")
				$follow = true;
			else if ($line == "Does not exist.\n")
				$exist = false;
		}

		fclose($fp);
		
		if ($follow)
			echo "You are now following <b>$fname</b>!";
		else if (!$exist)
			echo "$fname does not exist.";
		else
			echo "You are already following $fname!";
		
		
		// $ufile = fopen("users.txt", "r") or die("Couldn't open user file");
		// $userExists = false;
		// $friendFound = false;
		// while ( $line = fgets($ufile) ) {
			// $line = trim($line);
			// $users = explode(":", $line);
			// if ($users[0] == $fname){
				// $userExists = true;
			// }
			// if ($userExists)
				// break;
		// }
		// fclose($ufile);
		
		// // check if user is being followed already
		// if ($userExists){
			// $ffile = fopen("friends.txt", "r") or die("Couldn't open friend file");
			// while ( $line = fgets($ffile) ) {
				// $line = trim($line);
				// $friends = explode(":", $line);
				// $userFound = false;
				// if ($friends[0] == $_SESSION["username"]){
					// for ($i = 1; $i < count($friends); $i++){
						// if ($friends[$i] == $fname)
							// $friendFound = true;
						// if ($friendFound)
							// break;
					// }
					// $userFound = true;
				// }	
				// if ($userFound)
					// break;
			// }
			// fclose($ffile);
		// }
		
		// if (!$userExists)
			// echo "The username you typed in does not exist. Please try again";
		// elseif ($friendFound)
			// echo "You are already following this person!";
		// else {
			// $arr = file('friends.txt', FILE_IGNORE_NEW_LINES | FILE_SKIP_EMPTY_LINES);
			// $remove = "";
			// $file = fopen("friends.txt", "r+") or die("Couldn't open friend file");
			// while ( $line = fgets($file) ){
				// $line = trim($line);
				// $friends = explode(":", $line);
				// $userFound = false;
				// if ($friends[0] == $_SESSION["username"]){
					// $remove = $line;
					// $userFound = true;
				// }
				// if ($userFound)
					// break;
			// }
			// $arr = array_filter($arr, function($item) use ($remove) {
				// return $item != $remove;
			// });
			// file_put_contents('friends.txt', implode($arr, PHP_EOL));
			// $line = PHP_EOL . $line . ":" . $fname . PHP_EOL;
			// file_put_contents("friends.txt", $line, FILE_APPEND);
			
			// echo "You are now following <b>$fname</b>!";
		// }
		
		
		echo "<br/><br/>You will be redirected in 3 seconds or click <a href=\"follow.php\">here</a>.";
		header("refresh: 3; follow.php");
		?>
	</body>
</html>
