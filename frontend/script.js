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

loadAnnouncements();
