
//Time Updater
setInterval(()=>{
    fetch("/time")
        .then(res => res.text())
        .then(time => {
            document.getElementById("clock").textContent = time;
        });

},1000);