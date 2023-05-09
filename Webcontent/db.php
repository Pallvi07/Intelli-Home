<?php


header('Access-Control-Allow-Origin: *');

// Specify which request methods are allowed
header('Access-Control-Allow-Methods: PUT, GET, POST, DELETE, OPTIONS');

// Additional headers which may be sent along with the CORS request
header('Access-Control-Allow-Headers: X-Requested-With,Authorization,Content-Type');

// Set the age to 1 day to improve speed/caching.
header('Access-Control-Max-Age: 86400');

$con=mysqli_connect('localhost','sagars',"sagars","gasdetection") ;
$user=$_POST['fname,password,email'];
$qry1=mysqli_query($con,"insert into users(first_Name,password,email) values('$user')");
$qry2= "UPDATE input_values='fan,light,gas' WHERE id=1";
// $qry1 = mysqli_query($con, "UPDATE users SET first_Name = '$user' WHERE id = 1");


echo "<h1>Inerted-- $user";

if(isset($_GET['submit']))
{
   echo $fan_control= $_GET['fan_control'];
   echo $gas_control = $_GET['gas_control'];
   echo $light_control = $_GET['light_control'];
   

   $sql = "INSERT INTO `input_values` (fan, gas, light)
  VALUES ( $fan_control, $gas_control,$light_control)";
  
  if ($conn->query($qry2) === TRUE) {
  echo "Record updated successfully";
} else {
  echo "Error updating record: " . $conn->error;
}

if(mysqli_query($con, $sql)){  
 echo "Record inserted successfully";  
}else{  
echo "Could not insert record: ". mysqli_error($conn);  
}  
  
    
}
$qry=mysqli_query($con,"select *  from  users");
$row=mysqli_fetch_array($qry);
echo   "<h1>",$user;
?>