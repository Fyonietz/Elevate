//Time Updater
setInterval(()=>{
    fetch("/time")
        .then(res => res.text())
        .then(time => {
            document.getElementById("clock").textContent = time;
        });

},1000);

let users = [];

window.onload = function () {
  fetch('/JSON/app.json')
    .then(response => response.json())
    .then(data => {
      users = data;
      renderUsers();
    })
    .catch(err => console.error('Failed to load JSON:', err));
};


document.getElementById('os-path-btn').addEventListener('click', function() {
    fetch('/os/path', { method: 'POST' })
      .then(res => res.text())
      .then(data => console.log(data));
});

fetch('/JSON/app.json')
  .then(response => response.json())
  .then(data => {
    // app_lists is an array with one object
    const appObj = data.app_lists[0];
    // Loop through each key (app name) in the object
    for (const appName in appObj) {
      if (appObj.hasOwnProperty(appName)) {
        const location = appObj[appName].app_location;
        console.log(appName, location);
        // Example: print to HTML
        document.body.innerHTML += `<div>${appName}: ${location}</div>`;
      }
    }
  });