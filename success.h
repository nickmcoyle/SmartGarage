#define MULTI_LINE_STRING(a) #a
#define success_html_len strlen(success_html)
const char* success_html = MULTI_LINE_STRING(
<!doctype html>
<html>
    <head>
        <meta charset='utf-8'>        
        <meta name=viewport content="width=device-width, initial-scale=1, user-scalable=yes">
        <link rel='icon' href='data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAABAUlEQVRYR+2WwRHDMAgE40LSTcryM2WlmxSSjB5kZA1wB8jWJ37aEreCQ2a7LX62qv79tX/ej2c6TmpjE7XAozA0gCdagTEBkKB20syeA8DM1LasMPF+ANriaD09Q1vxDwAzBVF3SffQJkQBs9/TAFZ9o1kMAyCnSyZYkBCAiHvBR0AEQgMw4r0PehAPggKIigsIAwEBsuIjhJUFF6Aq3kMsB2gwGgTMAHIxewFZc8MlAF4p/wCXZEBmg7AJvY0R81kd0N7DDHibIxCpe6Afq7LtiMZ2eBVXIKb8C9g7fSwHIw49kAl62jyg/WI9E7KeoTxgCUVPq8UpAbBt6K1bDvAFI9/EIUVF+AgAAAAASUVORK5CYII=' />
        <title>Barry's Garage</title>
        <style> 
            .message {
              color: #22BA7B;
              background-color: #95e5a6;
              border: 2px solid #22BA7B;
              border-radius: 5%;      
            }                    
           
          </style>
    </head>
    <body>      
     
        <div id='wrapper'>         
          <div id="response-message" class="message">Login Success!</div>
        </div><!-- end wrapper-->       
  </body>
</html>
);
