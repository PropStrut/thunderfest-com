<?php

////
//// Take input from a javascript page view hit and log it
////

//// Don't forget to "chmod 777 log"!

define("LOG_FILE", "../log/googletracking.log");
$fp = fopen(LOG_FILE, "a+");
if ($fp) {
  if (flock($fp, LOCK_EX)) { // do an exclusive lock

    // Log the page view
    if (! strcmp($_GET["type"], "view")) {
      fwrite($fp, sprintf("VIEW:  %s  url=%s\r\n", date("M d H:i"), $_GET["url"]));
    }

    // Log the page view
    if (! strcmp($_GET["type"], "click")) {
      fwrite($fp, sprintf("CLICK: %s  url=%s  ref_url=%s\r\n", date("M d H:i"), $_GET["url"], $_GET["ref_url"]));
    }

    flock($fp, LOCK_UN); // release the lock
  }
  fclose($fp);
}

?>
