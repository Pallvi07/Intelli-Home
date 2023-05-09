<?php
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: PUT, GET, POST, DELETE, OPTIONS');
header('Access-Control-Allow-Headers: X-Requested-With,Authorization,Content-Type');
header('Access-Control-Max-Age: 86400');

$con = mysqli_connect('localhost', 'sagars', 'sagars', 'gasdetection');

// Check connection
if (mysqli_connect_errno()) {
    echo json_encode(array('error' => 'Failed to connect to MySQL: ' . mysqli_connect_error()));
    exit();
}

// Insert values into the database
if (isset($_POST['fname'])) {
    $user = $_POST['fname'];
    $qry1 = mysqli_query($con, "INSERT INTO users (first_Name) VALUES ('$user')");
    echo json_encode(array('message' => 'Inserted: ' . $user));
    exit();
}

// Retrieve the last values from the database
// $query = "SELECT light, fan, gas FROM input_values ORDER BY id DESC LIMIT 1";
$query = "SELECT light, fan,gas FROM input_values";
$result = mysqli_query($con, $query);

// Check if the query was successful
if ($result) {
    // Fetch the row from the result set
    $row = mysqli_fetch_assoc($result);

    // Access the values
    $light_control = $row['light'];
    $fan_control = $row['fan'];
    $gas_value = $row['gas'];

    // Create an array for the retrieved values
    $data = array(
        'light_control' => $light_control,
        'fan_control' => $fan_control,
        'gas_value' => $gas_value
    );

    // Output the retrieved values in JSON format
    echo json_encode($data);
} else {
    echo json_encode(array('error' => 'Error executing the query: ' . mysqli_error($con)));
}

mysqli_close($con);
?>
