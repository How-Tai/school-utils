const form = document.getElementById("feedback-form");
const status = document.getElementById("status");

form.addEventListener("submit", async event => {
	event.preventDefault();
	const button = form.querySelector("button");
	button.disabled = true;
	try {
		const res = await fetch("/api/feedback", {method: "POST", headers: {"Content-Type": "application/json"}, body: JSON.stringify({type: new FormData(form).get("type"), message: document.getElementById("message-input").value.trim()})});
		const body = await res.json().catch(() => ({}));
		if(!res.ok) throw new Error(body.error || "Could not submit feedback");
		form.reset();
		status.textContent = "Submitted. Thank you.";
	}
	catch(error) { status.textContent = error.message; }
	finally { button.disabled = false; }
});
