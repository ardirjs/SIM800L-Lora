<!DOCTYPE html>
<html lang="en" dir="ltr">
  <head>
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
    <title>SIM800L - Lora</title>
    <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no"/>
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no"/>
    <meta name="theme-color" content="#343a40"/>
    <meta name="msapplication-navbutton-color" content="#343a40"/>
    <meta name="apple-mobile-web-app-status-bar-style" content="#343a40"/>
    <meta name="programmer" content="rjsdevs, https://github.com/rjsdevs"/>
    <meta name="description" content="rjsdevs, https://github.com/rjsdevs"/>
    <meta name="keywords" content="rjsdevs, https://github.com/rjsdevs"/>
    <meta name="author" content="rjsdevs, https://github.com/rjsdevs"/>
    <meta name="title" content="rjsdevs, https://github.com/rjsdevs"/>
    <meta name="copyright" content="rjsdevs, https://github.com/rjsdevs"/>
    <meta Http-Equiv="Expires" content="0"/>
    <meta Http-Equiv="Pragma" content="No-Cache"/>
    <meta Http-Equiv="Cache-Control" content="No-Cache"/>
    <meta Http-Equiv="Cache-Control" content="No-Store"/>
    <meta Http-Equiv="Content-Language" content="En-Us"/>
    <meta Http-Equiv="X-UA-Compatible" content="IE=Edge"/>
    <link href="https://rjsdevs.github.io/web-dependencies/fonts/roboto-slab/roboto-slab.css" rel="stylesheet" type="text/css">
    <link href="https://rjsdevs.github.io/web-dependencies/fonts/russo-one/russo-one.css" rel="stylesheet" type="text/css">
    <link href="https://rjsdevs.github.io/web-dependencies/fonts/teko/teko.css" rel="stylesheet" type="text/css">
    <link href="https://rjsdevs.github.io/web-dependencies/bootstrap/css/bootstrap.min.css" rel="stylesheet" type="text/css">
    <style media="screen">
      .header {margin-bottom: 2rem; box-shadow: 2px 2px 5px #343a40;}
      .header-title {padding: 2rem 0; color: #fff; font-size: 1.5em; font-family: 'Russo One', sans-serif;}
      .section {margin-bottom: 2rem;}
      .data-node {padding: 1rem; border-radius: .7rem; box-shadow: 2px 2px 5px #343a40;}
      .btn {width: 100%; font-weight: 900; font-family: 'Roboto Slab', serif;}
      .table {margin: 0; font-size: .8em; font-family: 'Roboto Slab', serif;}
    </style>
  </head>
  <body>
    <div class="header bg-dark">
      <div class="container">
        <div class="d-flex">
          <div class="p flex-fill">
            <div class="header-title">SIM800L - Lora</div>
          </div>
        </div>
      </div>
    </div>
    <div class="section bg-white">
      <div class="container">
        <div class="d-flex">
          <div class="p flex-fill">
            <div class="data-node bg-light">
              <input type="submit" id="turning" class="btn btn-danger" name="turning" value="Status"/>
              <table class="table table-sm" id="table">
                <thead><tr><th colspan="3"></th></tr></thead>
                <tbody id="nodeSensor">
                  <tr id="datetime"><td>The Latest Time</td><td>:</td><td></td></tr>
                  <tr id="location"><td>Location</td><td>:</td><td></td></tr>
                  <tr id="nodeId"><td>Id Node</td><td>:</td><td></td></tr>
                </tbody>
              </table>
            </div>
          </div>
        </div>
      </div>
    </div>
    <script src="https://rjsdevs.github.io/web-dependencies/jquery/jquery.min.js" charset="utf-8" type="text/javascript"></script>
    <script src="https://rjsdevs.github.io/web-dependencies/popper/popper.min.js" charset="utf-8" type="text/javascript"></script>
    <script src="https://rjsdevs.github.io/web-dependencies/bootstrap/js/bootstrap.min.js" charset="utf-8" type="text/javascript"></script>
    <script type="text/javascript">
      $(document).ready(function() {
        jQuery.ajax({url: 'loads.php', cache: false, success: function(result) {
          result = JSON.parse(result);
          $('#table>thead>tr>th').html(result.region);
          $('#location>td:nth-child(3)').html('<a href="' + result.maplink + '" target="_blank">See location</a>');
          $('#nodeId>td:nth-child(3)').html(result.id);
          $('#datetime>td:nth-child(3)').html(result.times + ' - ' + result.dates);
          result = JSON.parse(result.sensor);
          jQuery.each(result, function(name, object) {
            $('#nodeSensor').append('<tr id="node-' + name + '"><td>Node ' + (name + 1) + '</td><td>:</td><td>' + object + '</td></tr>');
          });
        }});
        setInterval(function() {
          jQuery.ajax({url: 'loads.php', cache: false, success: function(result) {
            result = JSON.parse(result);
            $('#datetime>td:nth-child(3)').html(result.times + ' - ' + result.dates);
            $('#turning').removeClass(result.switch == 1 ? 'btn-danger' : 'btn-success').addClass(result.switch == 1 ? 'btn-success' : 'btn-danger');
            result = JSON.parse(result.sensor);
            jQuery.each(result, function(name, object) {
              $('#node-' + name + '>td:nth-child(3)').html(object);
            });
          }});
        }, 1000);
        $('#turning').click(function() {
          jQuery.ajax({url: 'posts.php', cache: false, success: function(result) {
            jQuery.ajax({url: 'posts.php?switch=' + (result == 0 ? 1 : 0), cache: false});
          }})
        });
      });
    </script>
  </body>
</html>
