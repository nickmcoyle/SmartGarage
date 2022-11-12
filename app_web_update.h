#define MULTI_LINE_STRING(a) #a
#define web_update_html_len strlen(web_update_html)
const char* web_update_html = MULTI_LINE_STRING(
<!doctype html>
<html>
     <head>
        <meta charset='utf-8'>        
        <meta name=viewport content="width=device-width, initial-scale=1, user-scalable=yes">
        <link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAATCAYAAACKsM07AAAABGdBTUEAALGOfPtRkwAAACBjSFJNAAB6JQAAgIMAAPn/AACA6AAAdTAAAOpgAAA6lwAAF2+XqZnUAAACVUlEQVR4nGJkZGT0/////2QGBgYhIP7DgBswQWlGIP6LRx1MLUhNNUAAgTjngfg/Efg3EEcC8Uwi1YPwA4AAAlnwnkjFx4C+ZQBiDxIs+AAQQExQLxMDtoEsYGJiegpk/yRSDwNAADERVgIG34F4AzCuGP79+/cMyP5BrAUAAUSsBYeA+CrIAiD4AvTJc2ItAAggfBY0Aw1aCGVvBGJmILYDBhEoeB5CxV8wQFLKHVyGAAQQCH1gwIyc5aDwZmFhMQayXwGxDBBbAHEPVM9cqLpeaMTPxWIGOJIBAogFyaJvDJB8sBPoymyw95iYbgEtaQQGzZO/f/+mA4U0oGpfQ+ldIAuA8teB7EtAfASIRYE4COpjBoAAAqFPUNs8QIpBgJmZWQXIBqV3HnZ2dgYODg4WoGWXgfyDUBcXAPFTNjY2PiAGaZEEYgGoeToMkEQA9gFAALFAGSDXegJTiDaQyQZ0bQCQNgPihb9+/ToGdKEZVONZqAUvgGq2AMVBjlMH4ttA/A+IG4HYG2QGzPUAAYQrDmC4HaqmBcoHGQRKGCZA7AU1aBcDpJgxZIDkD5Q4AAggQhaA0nwnED+B8t8BXc8LCjogzQzE9lDxViBegC2SAQKIkAXo+CswKJWhwQTC9QTUfwAIIGIzGgywAeNJBMQAGs4KpHwIaQAIIFItACUKMShbDxjJ+oQ0AAQQCyEFWIA4qMgAYicgm5WQYoAAAvngGYkWOAOxAhBHEKH2K0AAAYOSMRrI6AS6iI8BkpYJAVBuBJWuPAy4a0CQw/8AE0QHQIABABUM5YAzH1mhAAAAAElFTkSuQmCC' alt=''/>
        <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>
        <title>Web Update Page</title>
        <style>
            body {                  
               color: white; 
               font-family: Arial;
               font-size: 12px;
               line-height: 1.2;   
               background-color: black;
            }
            div#wrapper {
              max-width: 312px;
              display: block;
              margin: 0 auto;     
            }
            div#info-wrapper {
              max-width: 312px;
              display: block;
              margin: 0 auto;      
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
            div#instructions {
              font-weight: bold;
            }        
            .container-form {              
              margin: 0 auto;                                    
              background-color: #282727;      
              width: 192px;              
              border: 1px solid #282727;
              border-radius: 2%;
              padding: 10px 10px 30px 10px; 
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
      <!-- WiFi enabled esp32 device -->   
        <header>  
            <div class="navbar">
              <h1>
              <i class="fa fa-floppy-o"></i>
              Firmware Update
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
          <div id='info-wrapper'>          
            <div class='container-form' id='uploadForm'>          
              <div id='instructions'>Upload a compiled binary .bin file made in the Arduino IDE to flash the firmware</div>
              <br>          
              <form method='POST' onsubmit="event.preventDefault(); return submit_form();" enctype='multipart/form-data' id='upload_form'>
                  <input type='file' id='upload_file' name='update'/>
                  <input type='submit' value='Update'/>
              </form>
              <br>        
              <div id='progress'>upload progress: 0%</div>
              <div id='status_message'></div>
             </div>         
            </div>
         </div><!-- end wrapper--> 
     <script type="text/javascript">
      window.submit_form = function(e) {
          var statusMsg = document.getElementById('status_message');     
          var file = document.getElementById('upload_file');          
          var xhr = new XMLHttpRequest();
          xhr.onreadystatechange = function() {
            if (xhr.readyState == 4) {
                if (xhr.status == 200) {
                    document.open();
                    document.write(xhr.responseText);
                    document.close();
                } else if (xhr.status == 0) {
                    statusMsg.innerHTML = "Server closed the connection abruptly!";                    
                } else {
                    statusMessage.innerHTML = xhr.status + " Error!\n" + xhr.responseText;                   
                }
            }
          };
          xhr.upload.addEventListener('progress', function(evt) {
              if (evt.lengthComputable) {
                  var per = evt.loaded / evt.total;
                  document.getElementById('progress').innerHTML = 'upload progress: ' + Math.round(per * 100) + '%';
                  if(per == 1) {
                    statusMsg.innerHTML = "Writing to flash...";
                  }
              }
          });          
          xhr.open('POST', document.location.origin + '/web-updater', true);          
          xhr.setRequestHeader('Content-Type', 'application/octet-stream');
          //send file without any html, only the binary data          
          xhr.send(file.files[0]);         
          return false;
      };          
     </script>
  </body>
</html>
);
