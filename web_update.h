#define MULTI_LINE_STRING(a) #a
#define web_update_html_len strlen(web_update_html)
const char* web_update_html = MULTI_LINE_STRING(
<!doctype html>
<html>
     <head>
        <meta charset='utf-8'>        
        <meta name=viewport content="width=device-width, initial-scale=1, user-scalable=yes">
        <link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABAUlEQVRYR+2WwRHDMAgE40LSTcryM2WlmxSSjB5kZA1wB8jWJ37aEreCQ2a7LX62qv79tX/ej2c6TmpjE7XAozA0gCdagTEBkKB20syeA8DM1LasMPF+ANriaD09Q1vxDwAzBVF3SffQJkQBs9/TAFZ9o1kMAyCnSyZYkBCAiHvBR0AEQgMw4r0PehAPggKIigsIAwEBsuIjhJUFF6Aq3kMsB2gwGgTMAHIxewFZc8MlAF4p/wCXZEBmg7AJvY0R81kd0N7DDHibIxCpe6Afq7LtiMZ2eBVXIKb8C9g7fSwHIw49kAl62jyg/WI9E7KeoTxgCUVPq8UpAbBt6K1bDvAFI9/EIUVF+AgAAAAASUVORK5CYII=' />
        <title>Barry's Garage</title>
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
            h1 {    
              width: auto;              
              margin-top: 0px;
              margin-bottom: 0px;
              border: 10px solid #22BA7B;
              background-color: #22BA7B;
              color: black;
              font-size: 2em;
              text-align: center;
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
            input#doorBtn {         
              background: transparent;
              background-color:none;      
            }
            input#doorBtn:hover {
              cursor: pointer;
            }
            .container-form {              
              margin: 0 auto;                                    
              background-color: #282727;      
              width: 192px;
              height: 120px;
              border: 1px solid #282727;
              border-radius: 2%; 
            }            
          </style>
    </head>  
    <body>
      <!-- WiFi enabled garage door opener webpage -->
      <header>  
        <h1>
        <img src='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACIAAAAPCAYAAACBdR0qAAADVElEQVRIS7VUS2gTURQ9701n8reJbdqYprVfpFbBLtwJYkEQpFCQitYiXYiI1M9C1FXtwoULXSguXLkQ2o0rQQRBEAR3IopERdvaT9rGpjWpk89kpvNG3jhJpzHxU/AtQt7c++4995x7L/EGsCudxDtsPCKAtSqBqGu6wf/bD+EXj0faDQlEoIZT15HPpLQof2M5CgC4H5Mk7KDUqRiGUkUI1hQDAgyIUJHkYQBMA9C5s1GSCO1tvlsTk/KlcjZJQqeq4mM5G0/u94vdqZT2ujTmn+5FIP6I60guoQzl80Zv6/bAiamZ5HghWW3Ee2A5ln5uBWuxqjALqPOJk0uy1mbZSMArjifT2nHrblACxgyTHZNJ+3cewOAMEcTtjNQLFLd1hmPhsOfgwkLmma1qu58EQOM2YsYBtfnRYEC8k0hqww0trqPzX3IP/8REwW5PUAfgJoCT1dXi3tVV7VUFIBEA3wBkCyzYgTglel9R2VBjs7tvbjr7aDNAKKV4wBgGa2udPcvLCpei0D92wJyBdgCfCAEzqV33o011jhuzS/nLke2ewdhMZmwzQGoAjAC4EAj79iUX5Je/AeIDsFpOGreD3s3m2dltrZ7Bxam/AsILY/ZKWwGcB3AxGBS7EwntTQUgnAGHJQ1nzN4jRBQwpukYCDe7+xYqSDM6Osrf/LITTPrdboSzWVzlYKwRrASEg+fznxYImL5RGiJQjOkMA43bPIfmFjNPSxNyENFolHR1dW1YG+bSsUYrCOAagGG/X9yTSmlvKzDC3/CKdJdEtZzK+BQVglJRwD1Nx+lQg7cnPl8c+SKeAhulYEqlucB7RPJJnaqs2peWCZgAxPi5D/i2VV0SUXOqwWUqNrXXQe+k8+xcfZO79+ts9rGdkf7+fi4rStngoNaBONECBVcAnNnZEYppOvswMbV00Mpg+pk/PtRCNhOvCJRoOjPWGanG1pDLeTEeV0ZCEffheCz7xA6kkizcx85ICMB1AKfKjJxdwqK5zPgWAwaDzv2JhPKilJECGxWlcWxxdOS/5ycAuFxV6CaSwy95hMVUIltoWtLaWPNNEKlQBWoQgSrvPy/yfSI3NdRN55QM7zHIsiKpmg7mRQgyVv6FEe6rA+Zi+q/nd9L8ALkFbZWY+RwMAAAAAElFTkSuQmCC' alt=''>
        Barry's Garage
        </h1>               
      </header>
        <div id='wrapper'>
          <div id='info-wrapper'>          
            <div class='container-form' id='uploadForm'>    
                <form method='POST' onsubmit="event.preventDefault(); return submit_form();" enctype='multipart/form-data' id='upload_form'>
                    <input type='file' id='upload_file' name='update'/>
                    <input type='submit' value='Update'/>
                </form>        
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
          xhr.open('POST', document.location.origin + '/update', true);          
          xhr.setRequestHeader('Content-Type', 'application/octet-stream');
          //send file without any html, only the binary data          
          xhr.send(file.files[0]);         
          return false;
      };          
     </script>
  </body>
</html>
 );
