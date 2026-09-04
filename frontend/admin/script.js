const feedback = document.getElementById("feedback");
const announcements = document.getElementById("admin-announcements");

async function api(url, options = {}) {
	const res = await fetch(url, options);
	const body = await res.json().catch(() => ({}));
	if(!res.ok) throw new Error(body.error || "Request failed");
	return body;
}

async function loadAnnouncements() {
	const data = await api("/api/admin/announcements");
	announcements.innerHTML = "";
	if(data.announcements.length === 0) announcements.textContent = "No announcements.";
	for(const item of data.announcements) {
		const card = document.createElement("article");
		const heading = document.createElement("h3");
		const message = document.createElement("p");
		const deleteButton = document.createElement("button");
		heading.textContent = item.title;
		message.textContent = item.message;
		deleteButton.textContent = "Delete";
		deleteButton.addEventListener("click", async () => {
			await api(`/api/admin/announcements/${item.id}`, {method: "DELETE"});
			await loadAnnouncements();
		});
		card.append(heading, message, deleteButton);
		announcements.append(card);
	}
}

async function load() {
	const user = await api("/api/account/me");
	if(!user.loggedIn || !["admin", "owner"].includes(user.role)) {
		document.querySelector("main").innerHTML = "<h1>Admin access required</h1><a href='/login'>Login</a>";
		return;
	}
	document.getElementById("identity").textContent = `${user.displayName} · ${user.role}`;
	const data = await api("/api/admin/feedback");
	feedback.innerHTML = "";
	if(data.feedback.length === 0) feedback.textContent = "No feedback yet.";
	for(const item of data.feedback) {
		const card = document.createElement("article");
		const heading = document.createElement("h3");
		const message = document.createElement("p");
		const time = document.createElement("small");
		heading.textContent = item.type === "complaint" ? "Teacher complaint" : "Suggestion";
		message.textContent = item.message;
		time.textContent = item.createdAt;
		card.append(heading, message, time);
		feedback.append(card);
	}
	await loadAnnouncements();
}

document.getElementById("announcement-form").addEventListener("submit", async event => {
	event.preventDefault();
	const status = document.getElementById("announcement-status");
	try {
		await api("/api/admin/announcements", {method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({title: document.getElementById("title").value.trim(), message: document.getElementById("announcement-message").value.trim()})});
		event.target.reset();
		status.textContent = "Announcement posted.";
		await loadAnnouncements();
	}
	catch(error) { status.textContent = error.message; }
});

load().catch(error => feedback.textContent = error.message);
