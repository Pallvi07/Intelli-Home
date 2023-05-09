<?php
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: PUT, GET, POST, DELETE, OPTIONS');
header('Access-Control-Allow-Headers: X-Requested-With, Authorization, Content-Type');
header('Access-Control-Max-Age: 86400');

$con = mysqli_connect('localhost', 'sagars', 'sagars', 'gasdetection');

$name = $_GET['Name'];
$password = $_GET['password'];

// Match input values with the entire database
$query = "SELECT COUNT(*) AS match_count, Name, password FROM user WHERE Name = '$name' AND password = '$password'";
$result = mysqli_query($con, $query);

if ($result) {
    $row = mysqli_fetch_assoc($result);
    $matchCount = $row['match_count'];
    $response = array(
        'status' => ($matchCount > 0) ? 1 : 0,
        'name' => $row['Name'],
        'password' => $row['password']
    );
    echo json_encode($response);
} else {
    echo json_encode(array('status' => 0, 'error' => mysqli_error($con)));
}

mysqli_close($con);
?>
