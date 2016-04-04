<!doctype html>
<html>
	<head>
		<title>Register for Chirper</title>
		<meta charset = "UTF-8">
	</head>
	<body>
		<h1><center>Welcome to Chirper!</center></h1>
		<p>To make an account, please enter in your desired username and password below:</p>
		<form action = "registerResult.php" method="POST">
			Username:
			<input type = "text" name="username">
			<br>
			<br>
			Password:
			<input type = "password" name="password">
			<br>
			<br>
			<input type = "submit" value = "Register!">
			<br>
			<p><a href = "index.php">Return to login.</a></p>
		</form>
	</body>
</html>