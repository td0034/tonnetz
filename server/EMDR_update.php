<?php
// Update the database with the slider value, username, and target device
$slider1 = $_GET['slider1'];
$slider2 = $_GET['slider2'];
$On_off = $_GET['On_off'];
$username = $_GET["username"];
$targetDevice = $_GET["targetDevice"];

// Replace "localhost", "username", "password", and "database" with your MySQL server information
$mysqli = new mysqli("localhost", "tom", "patchwork", "slider_values");

if ($mysqli->connect_errno) {
  echo "Failed to connect to MySQL: " . $mysqli->connect_error;
  exit();
}

// Prepare the SQL statement with placeholders
$query = "INSERT INTO sliderData (username, targetDevice, On_off, slider1, slider2) VALUES (?, ?, ?, ?, ?)";

// Create a prepared statement with the SQL query
$stmt = $mysqli->prepare($query);

// Bind the values to the placeholders
$stmt->bind_param("sssss", $username, $targetDevice, $On_off, $slider1, $slider2);

// Execute the statement and check for errors
if (!$stmt->execute()) {
  echo "Error: " . $stmt->error;
} else {
  echo "New record created successfully";
}

$stmt->close();
$mysqli->close();
?>
