<?php
// Connect to database
$servername = "localhost";
$username = "tom";
$password = "patchwork";
$dbname = "slider_values";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
  die("Connection failed: " . $conn->connect_error);
}

// Select the latest line of data from table
$sql = "SELECT * FROM sliderData ORDER BY id DESC LIMIT 1";
$result = $conn->query($sql);

// Check if query was successful
if ($result->num_rows > 0) {
  // Fetch the result as an associative array
  $row = $result->fetch_assoc();
  // Send the data as a JSON string
  echo json_encode($row);
} else {
  // No data found
  echo "0 results";
}

// Close connection
$conn->close();
?>
