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