const form = document.getElementById("signup-form");
const message = document.getElementById("message");

form.addEventListener("submit", async (event) => {
	event.preventDefault();
	message.hidden = true;

	try {
		const res = await fetch("/api/account/signup", {
			method: "POST",
			headers: {"Content-Type": "application/json"},
			body: JSON.stringify({displayName: document.getElementById("display-name").value.trim(), username: document.getElementById("username").value.trim(), password: document.getElementById("password").value})
		});
		const body = await res.json().catch(() => ({}));
		if(!res.ok) throw new Error(body.error || "Could not create account");
		location.href = "/";
	}
	catch(error) {
		message.textContent = error.message;
		message.className = "error";
		message.hidden = false;
	}
});
