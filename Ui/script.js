// ================================
// 1. Fullscreen Toggle (Alt + Shift)
// ================================
document.addEventListener("keydown", function (e) {
  if (e.altKey && e.shiftKey) {
    if (!document.fullscreenElement) {
      document.documentElement.requestFullscreen().catch((err) => {
        console.error(`Error attempting to enable fullscreen: ${err.message}`);
      });
    } else {
      document.exitFullscreen();
    }
  }
});

// ================================
// 2. Disable Right Click Context Menu
// ================================
document.addEventListener("contextmenu", function (e) {
  e.preventDefault(); // Prevent default context menu
});

// ================================
// 3. Custom Context Menu Handling
// ================================
const menu = document.getElementById("custom-menu");

document.addEventListener("contextmenu", function (e) {
  e.preventDefault();

  const menuWidth = menu.offsetWidth;
  const menuHeight = menu.offsetHeight;

  let left = e.pageX;
  let top = e.pageY;

  // Keep menu within screen bounds
  if (left + menuWidth > window.innerWidth) {
    left = window.innerWidth - menuWidth - 10;
  }
  if (top + menuHeight > window.innerHeight) {
    top = window.innerHeight - menuHeight - 10;
  }

  menu.style.left = `${left}px`;
  menu.style.top = `${top}px`;
  menu.classList.remove("hidden");
});

document.addEventListener("click", function () {
  menu.classList.add("hidden"); // Hide menu on click anywhere
});

// ================================
// 4. Time Updater (Fetch from Server Every Second)
// ================================
setInterval(() => {
  fetch("/time")
    .then((res) => res.text())
    .then((time) => {
      document.getElementById("clock").textContent = time;
    })
    .catch((e) => {
      console.error("Time fetch error:", e);
    });
}, 1000);

// ================================
// 5. App Management System
// ================================
let apps = [];
let filteredApps = [];

window.onload = loadApps;

function add_app() {
  fetch("/os/path", { method: "POST" })
    .then((res) => res.text())
    .then(() => {
      setTimeout(loadApps, 100);
    });
}

function loadApps() {
  fetch("/JSON/app.json?" + Date.now())
    .then((res) => res.json())
    .then((data) => {
      apps = data.app_lists.map((obj) => {
        const name = Object.keys(obj)[0];
        return {
          name,
          location: obj[name].app_location,
          image: obj[name].app_image,
        };
      });
      renderApps();
    });
}

function renderApps(appList = apps) {
  const listDiv = document.getElementById("apps-list");
  listDiv.innerHTML = "";

  appList.forEach((app, idx) => {
    const appDiv = document.createElement("div");
    appDiv.innerHTML = `
      <button 
        style="background-image:url('${app.image}');background-repeat:no-repeat;background-position:center" 
        class="btn"  
        onclick="runApps('${app.name.replace(/'/g, "\\'")}')">
      </button>
      <button onclick="deleteApp(${idx})" class="del-btn">X</button>
    `;
    listDiv.appendChild(appDiv);
  });
}

function runApps(name) {
  fetch("/run", {
    method: "POST",
    headers: { "Content-Type": "application/x-www-form-urlencoded" },
    body: "name=" + encodeURIComponent(name),
  });
}

window.deleteApp = function (idx) {
  if (confirm("Are you sure you want to delete this app?")) {
    const appName = apps[idx].name;
    fetch("/delete", {
      method: "POST",
      headers: { "Content-Type": "application/x-www-form-urlencoded" },
      body: "name=" + encodeURIComponent(appName),
    })
      .then((res) => res.text())
      .then(() => {
        setTimeout(loadApps, 100);
      });
  }
};

function toggle_delete() {
  const delBtns = document.querySelectorAll(".del-btn");
  delBtns.forEach((btn) => {
    btn.style.display =
      btn.style.display === "none" || btn.style.display === ""
        ? "block"
        : "none";
  });
}

// ================================
// 6. App Search Menu & Keyboard Navigation
// ================================
const searchInput = document.getElementById("search-input");
const searchResults = document.getElementById("search-results");
const searchMenu = document.querySelector(".app-search-menu");

let currentMatches = [];
let selectedIndex = -1;

if (searchInput && searchResults) {
  searchInput.addEventListener("input", function () {
    const query = this.value.trim().toLowerCase();
    searchResults.innerHTML = "";
    currentMatches = [];
    selectedIndex = -1;
    if (query === "") return;

    // Fetch from search.json every time the user types
    fetch("/JSON/search.json?" + Date.now())
      .then((res) => res.json())
      .then((data) => {
        const allSearchApps = data.app_lists.map((obj) => {
          const name = Object.keys(obj)[0];
          return {
            name,
            location: obj[name].app_location,
            image: obj[name].app_image,
          };
        });

        currentMatches = allSearchApps.filter((app) =>
          app.name.toLowerCase().includes(query)
        );

        currentMatches.forEach((app, idx) => {
          const li = document.createElement("li");
          li.textContent = app.name;
          li.style.cursor = "pointer";
          li.onclick = function () {
            runApps(app.name);
          };
          searchResults.appendChild(li);
        });
      })
      .catch((err) => {
        console.error("Search fetch error:", err);
      });
  });

  // Key navigation (arrow keys + enter)
  searchInput.addEventListener("keydown", function (e) {
    const items = searchResults.querySelectorAll("li");

    if (e.key === "ArrowDown") {
      e.preventDefault();
      if (currentMatches.length === 0) return;
      selectedIndex = (selectedIndex + 1) % currentMatches.length;
      updateSelection(items);
    } else if (e.key === "ArrowUp") {
      e.preventDefault();
      if (currentMatches.length === 0) return;
      selectedIndex = (selectedIndex - 1 + currentMatches.length) % currentMatches.length;
      updateSelection(items);
    } else if (e.key === "Enter") {
      e.preventDefault();
      if (currentMatches.length > 0) {
        const idx = selectedIndex >= 0 ? selectedIndex : 0;
        runApps(currentMatches[idx].name);
        searchResults.innerHTML = "";
        searchInput.value = "";
        searchMenu.classList.remove("active");
        selectedIndex = -1;
      }
    }
  });
}

function updateSelection(items) {
  items.forEach((li, idx) => {
    if (idx === selectedIndex) {
      li.style.background = "#D72638";
      li.style.color = "#fff";
      li.scrollIntoView({ block: "nearest" }); // optional scrolling
    } else {
      li.style.background = "#333";
      li.style.color = "#fff";
    }
  });
}

// ================================
// 7. Search Menu Hotkeys
// ================================
document.addEventListener("keydown", function (e) {
  // Ctrl + / to open/close
  if (e.altKey) {
    searchMenu.classList.toggle("active");
    const input = searchMenu.querySelector("input");
    if (searchMenu.classList.contains("active") && input) input.focus();
  }

  // Escape to close
  if (e.key === "Escape") {
    searchMenu.classList.remove("active");
  }
});

// Close on outside click
document.addEventListener("mousedown", function (e) {
  if (searchMenu.classList.contains("active") && !searchMenu.contains(e.target)) {
    searchMenu.classList.remove("active");
  }
});
