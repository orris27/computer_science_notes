<?php
    $memcache = new Memcache;
    $memcache->connect('172.19.28.82',11211) or die ("Could not connect");
    $memcache->set('key1','orris');
    $val1 = $memcache->get('key1');
    echo "start";
    echo $val1."<br>";
    echo $val1;
    echo"ok";
    echo "one";
?>
