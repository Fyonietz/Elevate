// Time Updater
setInterval(() => {
    fetch("/time")
        .then(res => res.text())
        .then(time => {
            document.getElementById("clock").textContent = time;
        });
}, 1000);

let apps = [];

window.onload = loadApps;

document.getElementById('os-path-btn').addEventListener('click', function() {
    fetch('/os/path', { method: 'POST' })
      .then(res => res.text())
      .then(data => { 
        setTimeout(loadApps, 500); // Wait a bit before loading apps
      });
});

function loadApps() {
  fetch('/JSON/app.json?' + Date.now())
    .then(res => res.json())
    .then(data => {
      apps = data.app_lists.map(obj => {
        const name = Object.keys(obj)[0];
        return {
          name,
          location: obj[name].app_location
        };
      });
      renderApps();
    });
}

function renderApps() {
  const listDiv = document.getElementById('apps-list');
  listDiv.innerHTML = '';
  apps.forEach((app, idx) => {
    const appDiv = document.createElement('div');
    appDiv.innerHTML = `
      <b>${app.name}</b>
      <button onclick="runApps('${app.name.replace(/'/g, "\\'")}')">Run</button>
      <button onclick="deleteApp(${idx})">Delete</button>
    `;
    listDiv.appendChild(appDiv);
  });
}

function runApps(name) {
  fetch('/run', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded'
    },
    body: encodeURIComponent(name)
  });
}

window.deleteApp = function(idx) {
  if (confirm("Are you sure you want to delete this app?")) {
    const appName = apps[idx].name;
    fetch('/delete', {
      method: 'POST',
      headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
      body: 'name=' + encodeURIComponent(appName)
    })
    .then(res => res.text())
    .then(() => {
      setTimeout(loadApps, 300); // Reload the list after backend update
    });
  }
};

