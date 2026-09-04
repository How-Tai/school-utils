async function loadAccountButton() {
	const button = document.getElementById("account-button");

	try {
		const res = await fetch("/api/account/me");
		const data = await res.json();

		if(!data.loggedIn) return;

		button.textContent = "Logout";
		button.href = "#";
		button.addEventListener("click", async (event) => {
			event.preventDefault();
			button.textContent = "Logging out...";
			button.style.pointerEvents = "none";

			try {
				await fetch("/api/account/logout", { method: "POST" });
			}
			finally {
				location.reload();
			}
		});
	}
	catch {}
}

async function loadAnnouncements() {
	const container = document.getElementById("announcements");
	try {
		const res = await fetch("/api/announcements");
		const data = await res.json();
		container.innerHTML = "";
		if(!res.ok) throw new Error(data.error || "Could not load announcements");
		if(data.announcements.length === 0) container.textContent = "No announcements.";
		for(const item of data.announcements) {
			const card = document.createElement("article");
			const title = document.createElement("h3");
			const message = document.createElement("p");
			title.textContent = item.title;
			message.textContent = item.message;
			card.append(title, message);
			container.append(card);
		}
	}
	catch(error) { container.textContent = error.message; }
}

loadAccountButton();
loadAnnouncements();
