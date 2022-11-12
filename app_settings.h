#define MULTI_LINE_STRING(a) #a
#define settings_html_len strlen(settings_html)
const char* settings_html = MULTI_LINE_STRING(
<!doctype html>
<html>
    <head>        
        <meta charset='utf-8'>        
        <meta name=viewport content="width=device-width, initial-scale=1, maximum-scale=1.0, user-scalable=yes">        
        <link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAATCAYAAACKsM07AAAABGdBTUEAALGOfPtRkwAAACBjSFJNAAB6JQAAgIMAAPn/AACA6AAAdTAAAOpgAAA6lwAAF2+XqZnUAAACVUlEQVR4nGJkZGT0/////2QGBgYhIP7DgBswQWlGIP6LRx1MLUhNNUAAgTjngfg/Efg3EEcC8Uwi1YPwA4AAAlnwnkjFx4C+ZQBiDxIs+AAQQExQLxMDtoEsYGJiegpk/yRSDwNAADERVgIG34F4AzCuGP79+/cMyP5BrAUAAUSsBYeA+CrIAiD4AvTJc2ItAAggfBY0Aw1aCGVvBGJmILYDBhEoeB5CxV8wQFLKHVyGAAQQCH1gwIyc5aDwZmFhMQayXwGxDBBbAHEPVM9cqLpeaMTPxWIGOJIBAogFyaJvDJB8sBPoymyw95iYbgEtaQQGzZO/f/+mA4U0oGpfQ+ldIAuA8teB7EtAfASIRYE4COpjBoAAAqFPUNs8QIpBgJmZWQXIBqV3HnZ2dgYODg4WoGWXgfyDUBcXAPFTNjY2PiAGaZEEYgGoeToMkEQA9gFAALFAGSDXegJTiDaQyQZ0bQCQNgPihb9+/ToGdKEZVONZqAUvgGq2AMVBjlMH4ttA/A+IG4HYG2QGzPUAAYQrDmC4HaqmBcoHGQRKGCZA7AU1aBcDpJgxZIDkD5Q4AAggQhaA0nwnED+B8t8BXc8LCjogzQzE9lDxViBegC2SAQKIkAXo+CswKJWhwQTC9QTUfwAIIGIzGgywAeNJBMQAGs4KpHwIaQAIIFItACUKMShbDxjJ+oQ0AAQQCyEFWIA4qMgAYicgm5WQYoAAAvngGYkWOAOxAhBHEKH2K0AAAYOSMRrI6AS6iI8BkpYJAVBuBJWuPAy4a0CQw/8AE0QHQIABABUM5YAzH1mhAAAAAElFTkSuQmCC' alt=''/>
        <title>App Settings</title>        
        <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>
    <style>    
    body {         
       color: white; 
       font-family: Arial;
       font-size: 12px;
       line-height: 1.2;   
       background-color: black;
    }    
    div#wrapper {
      max-width: 310px;
      display: block;
      margin: 0 auto;     
    }
    div#info-wrapper {
      max-width: 310px;
      display: block;
      margin: 0 auto;
      margin-bottom: 7px;      
    }  
    header {    
      width: auto;      
      margin-top: 0px;
      margin-bottom: 0px;
      border: 10px solid #22BA7B;
      background-color: #22BA7B;
      color: black;
      font-size: 1em;
      text-align: center;
    }
    h1{
      display: inline;
    }
    .alert {
      position: absolute;
      top: 8px;        
      width: 100%;
      font-size: 14px;
      padding: 15px;
      margin-bottom: 10px;
      border: 1px solid transparent;
      border-radius: 4px;           
    }  
    div#success-message {
      width: 50%;
      display: none;           
      color: #3c763d;
      background-color: #dff0d8; 
      border-color: #d6e9c6;      
    }
    div#error-message {
      width: 50%;
      display: none;                 
      color: #a94442;
      background-color: #f2dede; 
      border-color: #ebccd1;    
    }
    .closeAlertBtn {
      padding: 0.5em;
      position: relative;
      bottom: 10px;
      left: 20px;
    } 
    .closeAlertBtn:hover {
      cursor: pointer;
    }     
    .rounded {
      display:block;
      width:70px;
      height:70px;      
      margin-top: 10%;
      margin-left: 10%;
      line-height: 65px;         
      text-align: center;
      text-decoration: none;     
      box-shadow: 0 0 3px gray;
      font-size: 13px;     
      border-radius: 50%;
    }
    .red {
      color: #ff0f0f;
      border: 2px solid #ff0f0f;
    }
    .grey {
      color: #969590;
      border: 2px solid #969590;
    }
    .yellow {
      color: #f4c542;
      border: 2px solid #f4c542;      
    }
    .green {
      color: #22BA7B;
      border: 2px solid #22BA7B;      
    }
    .blue {
      color: #4E64CC;
      border: 2px solid #4E64CC;     
    }
    a#power-status {         
      background: transparent;           
    }    
    a#power-status:hover {
      cursor: pointer;
    }
    .container-info {      
      margin-bottom: 5px;      
      display: inline-block;
      color: gray;     
      background-color: #282727;      
      width: 91px;
      height: 120px;
      border: 1px solid #282727;
      border-radius: 2%; 
    }
    .container-info label {
      font-size:10px;
      position: relative;
      left: 5px;
      top: 5px;
    }
    div#temperature-display {
      margin-left: 9px;
      margin-right: 9px;
    }         
    img#stream {
      max-width: 296px;
      clear:both;
      display: block;      
      margin: 0 auto;     
      border: 3px solid #282727;
      border-radius: 1%;  
    }
    figure {
      margin-left: 0px;     
    }  
    ul.input-list {
      list-style-type: none;
    }
    ul.input-list li {
      padding: 10px;
      position: relative;
      right: 10%;
      margin-bottom: 10px;
    }
    ul.input-list li label {
     font-weight: bold;
     position: relative;
     top: 6px;
     left: 20px;
    }
    ul.input-list li input {
      float: right; 
      width: 35%;
      padding: 5px;
      border: 2px outset white;     
    }
    .input-rounded {
      border: 1px solid #333;
      border-radius: 5px;
      padding: 2px;
      border: 5px solid       
    }
    .submit-button {
      float: right;
      color: #bbbaba;
      background-color: #282727;
      border: 1px solid #282727;
      border-radius: 5px;
      padding: 5px;
      margin: 5px;  
      background-image: linear-gradient(to bottom right, #000, #282727);
      box-shadow: 0 0 3px gray;
    }  

    //Navbar container
    .navbar {
      overflow: hidden;
    }
    
    //Links inside the navbar
    .navbar a {
      //float: left;
      font-size: 16px;
      color: white;
      text-align: center;
      padding: 14px 16px;
      text-decoration: none;
    }
    
    // The dropdown container 
    .dropdown {
      //float: left;
      display: inline-block;
      overflow: hidden;
    }
    
    //Dropdown button 
    .dropdown .dropbtn {
      font-size: 16px;
      border: none;
      outline: none;
      color: black;
      background-color: inherit;
      font-family: inherit; // Important for vertical align on mobile phones 
      margin: 0; // Important for vertical align on mobile phones
    }
    
    // Add a red background color to navbar links on hover 
    .navbar a:hover {
      background-color: black;
    }

    .dropdown:hover .dropbtn {
      background-color: white;
    }
    
     //Dropdown content (hidden by default) 
    .dropdown-content {
      display: none;
      position: absolute;
      background-color: #ddd;
      min-width: 160px;
      box-shadow: 0px 8px 16px 0px rgba(0,0,0,0.2);
      z-index: 1;
    }
    
    //Links inside the dropdown 
    .dropdown-content a {
      float: none;
      color: black;
      padding: 12px 16px;
      text-decoration: none;
      display: block;
      text-align: left;
    }
    
    //Add a grey background color to dropdown links on hover
    .dropdown-content a:hover {
      background-color: #868383;
    }
    
    //Show the dropdown menu on hover 
    .dropdown:hover .dropdown-content {
      display: block;
    }

        </style>
    </head>
    <body>
  <!-- WiFi enabled espresso PID settings webpage -->    
    <header>  
    <div class="navbar">
      <h1>
      <i class="fa fa-cog"></i>
      Settings
      </h1>   
        <div class="dropdown">
          <button class="dropbtn">
            <i class="fa fa-bars"></i>
          </button>
          <div class="dropdown-content">
            <a href="/"><i class="fa fa-coffee"></i> Home</a>
            <a href="/view-log"><i class="fa fa-exclamation-triangle"></i> Log</a>
            <a href="/web-updater-upload-page"><i class="fa fa-floppy-o"></i> Update</a>
            <a href="/settings"><i class="fa fa-cog"></i> Settings</a>
          </div>
        </div>  
      </div>                  
    </header>    
  <div id='wrapper'>             
   <div id='success-message' class='alert'>   
    <span>Success! </span>
    <i class="fa fa-close closeAlertBtn"></i>
    </div>
    <div id='error-message' class='alert'>
    <span>Error: </span>
    <i class="fa fa-close closeAlertBtn"></i>
    </div>
    
     <div id='input-wrapper'> 
     <ul class='input-list' id='form'>
      <li>
        <label for='appName'>App Name</label>
        <input class='input-rounded settings-value' id='app-name' type='text' name='appname'>
      </li>
      <li>
        <label for='runtimelimit'>Runtime Limit</label>
        <input class='input-rounded settings-value' id='runtime-limit' type='number' pattern='\d*' min=0 step=1 name='runtimelimit'>
      </li>
      <li>
        <label for='temperaturelimit'>Temperature Limit</label>
        <input class='input-rounded settings-value' id='temperature-limit' type='number' pattern='\d*' min=0 step='0.01'  name='temperaturelimit'>
      </li>
      <li>
        <label for='temperaturesensoroffset'>Thermocouple Offset</label>
        <input class='input-rounded settings-value' id='temperature-sensor-offset' type='number' pattern='\d*' min=0 step='0.01'  name='temperaturesensoroffset'>
      </li>
      <li>
        <label for='serialgraphingenabled'>Serial Graphing Enabled</label>
        <input class='input-rounded settings-value' id='serial-graphing-enabled' type='checkbox' name='serialgraphingenabled'>
      </li>  
      <li>
        <label for='loggingenabled'>Logging Enabled</label>
        <input class='input-rounded settings-value' id='logging-enabled' type='checkbox' name='loggingenabled'>
      </li>        
      <li>              
        <button class='submit-button' id='submitBtn' type='submit'>Submit</button>
        <button class='submit-button' id='defaultsBtn' type='submit'>Default Values</button>
      </li>
      </ul>     
     </div><!-- end input-wrapper div>
     
      </div><!-- end wrapper-->               
    
    <script type="text/javascript">            
      document.addEventListener('DOMContentLoaded', function (event) {
        "use strict";
        var baseHost = document.location.origin;           

        
        var getSettings = function() {
          // read values from server
          var xhr = new XMLHttpRequest();
          xhr.open('GET', document.location.origin + '/get-settings', true);
          xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhr.onload = function () {
            var json = JSON.parse(this.responseText);                                  
            for(var key in json) {          
              updateValue(key, json[key]); 
            }; 
          };
          xhr.send();
        };       
        
     
        //update values in the DOM
        var updateValue = function(el, value) {        
          var domElement = document.getElementById(el);
          if (domElement) {                      
            if (el === 'temperature') {         
                domElement.innerHTML = parseFloat(value.toPrecision(3)) + '&#176;F';
            } else if (el === 'power-status') {
              domElement.value = value;              
                if(value === 'ON') {                
                    domElement.classList.remove('grey');
                    domElement.classList.add('yellow');                    
                    domElement.innerHTML = 'ON&nbsp;&nbsp;<i class="fa fa-power-off" style="font-size:15px;"></i>';
                } else {                                      
                    domElement.innerHTML = 'OFF&nbsp;<i class="fa fa-power-off" style="font-size:15px;"></i>';
                    domElement.classList.remove('yellow');
                    domElement.classList.add('grey');
                }                              
              } else if (el === 'logging-enabled') {
              if (value === 1) {
                domElement.checked = true; 
              } else {
                domElement.checked = false; 
              }
            } else {
                domElement.value = value; 
                domElement.innerHTML = value;
            }
          }   
        };     
  
        document.getElementById('submitBtn').addEventListener('click', function () {
          var formData = document.getElementById('form').getElementsByTagName('input');
          var numElements = formData.length;
          var params = {numElements};
          for(var i = 0; i < numElements; ++i) {
            //if the input was a checkbox, you have to use the checked property to get the data, otherwise use value property            
            if(formData[i].type === 'checkbox') {
              params[i] = formData[i].name + ":" + formData[i].checked;
            } else {
              params[i] = formData[i].name + ":" + formData[i].value;
            }
          }          
          var xhr = new XMLHttpRequest();
          xhr.open('POST', document.location.origin + '/set-settings', true);
          xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhr.send(JSON.stringify(params));          
          xhr.onload = function () {
            var json = JSON.parse(this.responseText);          
           for(var key in json) {            
              if(key === 'success' && json[key]){              
                alertSuccess();
              } else {
                alertError(json[key]);
              }
            }
          };        
        });
  
        document.getElementById('defaultsBtn').addEventListener('click', function () { 
          if (confirm("All values will be reset to default, are you sure?")) {     
          var xhr = new XMLHttpRequest();
          xhr.open('POST', document.location.origin + '/set-default-settings', true);
          xhr.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');
          xhr.onload = function () {
            var json = JSON.parse(this.responseText);                  
            for(var key in json) {            
              if(key === 'success' && json[key]){              
                alertSuccess();
              } else {
                alertError(json[key]);
              }
            }
            getSettings();
          };        
          xhr.send();
          }
        });           
        
        var hideAlertElement = function(element) {
          document.getElementById('success-message').style.display = 'none';
          document.getElementById('error-message').style.display = 'none';
        };
        
        var alertSuccess = function() {
          var element = document.getElementById('success-message');        
          element.style.display = 'block';
          element.focus();        
          setTimeout(hideAlertElement, 5000);
        };
        
        var alertError = function(errorMsg) {
          var element = document.getElementById('error-message');    
          element.style.display = 'block';
          element.focus();
          var message = document.createTextNode(errorMsg);
          element.appendChild(message);     
          setTimeout(hideAlertElement, 10000);
        };         
        getSettings();

        var alertSpans = document.getElementsByClassName('closeAlertBtn');
        for(var i = 0; i<alertSpans.length; ++i) {    
          alertSpans[i].addEventListener('click', function() {
            hideAlertElement();
          });
        }
});
    </script>
    </body>
</html>
);
