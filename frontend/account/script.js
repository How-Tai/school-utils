const guest = document.getElementById("guest");
const profile = document.getElementById("profile");
const message = document.getElementById("message");

async function api(url, options = {}) {
	const res = await fetch(url, options);
	const body = await res.json().catch(() => ({}));
	if(!res.ok) throw new Error(body.error || "Request failed");
	return body;
}

async function refresh() {
	const user = await api("/api/account/me");
	guest.hidden = user.loggedIn;
	profile.hidden = !user.loggedIn;
	if(user.loggedIn) {
		document.getElementById("welcome").textContent = `Welcome, ${user.displayName}`;
		document.getElementById("role").textContent = `@${user.username} · ${user.role}`;
		document.getElementById("admin-link").hidden = !["admin", "owner"].includes(user.role);
	}
}

document.getElementById("login-form").addEventListener("submit", async event => {
	event.preventDefault();
	try {
		await api("/api/account/login", {method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({username: document.getElementById("login-username").value.trim(), password: document.getElementById("login-password").value})});
		message.textContent = "Logged in.";
		await refresh();
	}
	catch(error) { message.textContent = error.message; }
});

document.getElementById("signup-form").addEventListener("submit", async event => {
	event.preventDefault();
	try {
		await api("/api/account/signup", {method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({displayName: document.getElementById("signup-name").value.trim(), username: document.getElementById("signup-username").value.trim(), password: document.getElementById("signup-password").value})});
		message.textContent = "Account created.";
		await refresh();
	}
	catch(error) { message.textContent = error.message; }
});

document.getElementById("logout").addEventListener("click", async () => {
	await api("/api/account/logout", {method: "POST"});
	message.textContent = "Logged out.";
	await refresh();
});

refresh().catch(() => message.textContent = "Could not load account.");
