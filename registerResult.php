<!doctype html>
<html>
	<head>
		<title>Register for Chirper</title>
		<meta charset = "UTF-8">
	</head>
	<body>
		<h1><center>Welcome to Chirper!</center></h1>
		<?php
			$username = $_POST["username"];
			$password = $_POST["password"];
			
			$register = "register:User-" . $username . ":" . $password . PHP_EOL;
		
			$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
			if (!$fp) {
				$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
				if (!$fp){
					echo $errstr;
					exit(1);
				}
			}
			fwrite($fp, $register);
			
			$signUp = false; 
		
			while($line = fgets($fp)) {
				if($line == "Success!\n")
					$signUp = true;
			}

		   fclose($fp);
			
			// $ufile = fopen("users.txt", "r") or die("Couldn't open user file");
			// $userExists = false;
			// while ( $line = fgets($ufile) ){
				// $line = trim($line);
				// $users = explode(":", $line);
				// if ($users[0] == $username){
					// $userExists = true;
				// }
				// if ($userExists)
					// break;
			// }
			// fclose($ufile);
			
			if($signUp) {
				// $line = $username . ":" . $password . PHP_EOL;
				// file_put_contents("users.txt", $line, FILE_APPEND);
				// $line = $username . PHP_EOL;
				// file_put_contents("posts.txt", $line, FILE_APPEND);
				// file_put_contents("friends.txt", $line, FILE_APPEND);
				
				echo "Congratulations! Your account has been created!\n";
			}
		   else
				echo $username . " is already taken. Please try a different username.\n";
		   
			
		echo "<br/>You will be redirected in 3 seconds or click <a href=\"index.php\">here</a>.";
		header("refresh: 3; index.php");
		?>
	</body>
</html>
