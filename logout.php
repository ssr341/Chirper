<!doctype html>
<html>
	<head>
		<title>Logging Out</title>
		<meta charset = "UTF-8">
	</head>
	<body>
		<?php
			echo "<center><h1>Leaving so soon? :(</h1></center>";
			session_start();
			echo "Logging out of your account.<br/>";
			// remove all session variables
			session_unset();

			// destroy the session
			session_destroy(); 
			echo "You will be redirected in 3 seconds or click <a href=\"index.php\">here</a>.";
			header("refresh: 3; index.php");
		?>
	</body>
</html>