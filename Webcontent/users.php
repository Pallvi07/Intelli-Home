<?php
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: PUT, GET, POST, DELETE, OPTIONS');
header('Access-Control-Allow-Headers: X-Requested-With, Authorization, Content-Type');
header('Access-Control-Max-Age: 86400');

$con = mysqli_connect('localhost', 'sagars', 'sagars', 'gasdetection');

$name = $_POST['Name'];
$password = $_POST['password'];
$confirm_password = $_POST['confirm_password'];

$name = $_GET['Name'];
$password = $_GET['password'];
$confirm_password = $_GET['confirm_password'];

$sql = "INSERT INTO `user` (Name, password, confirm_password) VALUES ('$name', '$password', '$confirm_password')";

if (mysqli_query($con, $sql)) {
    echo "Record inserted successfully";
} else {
    echo "Could not insert record: " . mysqli_error($con);
}

$qry1 = mysqli_query($con, "SELECT * FROM user");
$row = mysqli_fetch_array($qry1);
// echo "<h1>", $row['Name'];
?>
