<!doctype html>
<html>
	<head>
		<title>Welcome to Chirper!</title>
		<meta charset = "UTF-8">
	</head>
	<body> 
		<?php
		session_start();
		echo "<center><h1>Welcome to Chirper!</h1></center>";
		
		echo "<h4>Welcome to Chirper! A website where you can keep up with the people you want and where people can keep up with you!</h4>";
		
		if(isset($_POST["username"]) && isset($_POST["password"])) {
			$user = $_POST["username"];
			$pass = $_POST["password"];
			$login = false;

			//check if entry exists
			// $file = fopen("users.txt", "r") or die("Couldn't open the file");
			// while ( $line = fgets($file) ) {
				// $line = trim($line);
				// $loginInfo = explode(":", $line);
				// if ($loginInfo[0] == $user && $loginInfo[1] == $pass){
					// $_SESSION["username"] = $user;
					// $login = true;
				// }
				// if ($login)
					// break;
			// }
			// fclose($file);
			
			$loginInfo = "login:" . $user . ":" . $pass . PHP_EOL;
			$fp = stream_socket_client("tcp://localhost:12345", $errno, $errstr, 5);
			if (!$fp) {
				$fp = stream_socket_client("tcp://localhost:54321", $errno, $errstr, 5);
				if (!$fp){
					echo $errstr;
					exit(1);
				}
			}
			fwrite($fp, $loginInfo);

			while($line = fgets($fp)) {
				//if ($line == "Success!\n")
		                if(strcmp($line, "Success!\n")==0){
					$login = true;
		                        $_SESSION["username"] = $user;
		                }
			}

			fclose($fp);
			
			// if match found
			if ($login){
				echo "Login successful. \n";
				echo "You will be redirected in 3 seconds or click <a href=\"homepage.php\">here</a>.";
				header("refresh: 3; homepage.php");
			}
			//if no match then tell them to try again
			else {
				echo 'Your username or password is incorrect.<br>Click <a href="index.php">here</a> to try again.';
			}
		}
		//if not then display login form
		else {
			echo "Enter your username and password below: <br /><br />\n";
			echo '<form action="index.php" method="POST">';
			echo "\n";
			echo 'Username: <input type="text" name="username" /><br /><br/>';
			echo 'Password: <input type="password" name="password" /><br /><br/>';
			echo '<input type="submit" value="Login" /></form><br/>';
			echo '<p><a href = "register.php">Want to create an account?</a></p>';
		}
		?>
	</body>
</html>
