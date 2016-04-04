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
		$fname = $_POST["remove"];
		$unfollowed = false;
		
		$unfollow = "unfollow:User-" . $username . ":" . $fname . PHP_EOL;

		$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
		if (!$fp) {
			$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
			if (!$fp){
				echo $errstr;
				exit(1);
			}
		}
		fwrite($fp, $unfollow);
		
		while($line = fgets($fp)) {
			if($line == "Success!\n")
				$unfollowed = true;
		}

		fclose($fp);
		
		if ($unfollowed)
			echo "You have removed <b>$fname</b>!";
		else
			echo "You are not currently not following <b>$fname</b>.";
			
	
		// check if user exists
		// $fname = $_POST["remove"];
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
		// elseif (!$friendFound)
			// echo "You are not currently not following <b>$fname</b>.";
		// elseif ($friendFound){
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
			// $search = ":" . $fname;
			// $line = str_replace($search, '', $line);
			// $line = PHP_EOL . $line . "\n";
			// file_put_contents("friends.txt", $line, FILE_APPEND);
			
			// echo "You have removed <b>$fname</b>!";
		// }
		
		
		echo "<br/><br/>You will be redirected in 3 seconds or click <a href=\"follow.php\">here</a>.";
		header("refresh: 3; follow.php");
		?>
	</body>
</html>
