function bodyloaded()
{
	var images=document.getElementsByTagName("img");
	for(var i=0; i<images.length; i++)
	{
        images[i].addEventListener("mouseover", function(e){
            if(e.target.src.endsWith(".c.png")) e.target.src=e.target.src.replace(".c.png",".png");
            if(e.target.src.endsWith(".c.jpg")) e.target.src=e.target.src.replace(".c.jpg",".jpg");
            if(e.target.src.endsWith(".c.webp")) e.target.src=e.target.src.replace(".c.webp",".webp");
        });
	}
}

