const form = document.getElementById("attendance-form");
const input = document.getElementById("student-id");
const message = document.getElementById("message");
const button = form.querySelector("button");

function showMessage(text, error = false) {
	message.textContent = text;
	message.className = error ? "error" : "success";
	message.hidden = false;
}

form.addEventListener("submit", async (event) => {
	event.preventDefault();

	const uid = input.value.trim();

	if(!uid) {
		showMessage("Please enter your student ID.", true);
		return;
	}

	button.disabled = true;
	message.hidden = true;

	try {
		const res = await fetch("/dschool/attendance", {
			method: "POST",
			headers: {
				"Content-Type": "application/json"
			},
			body: JSON.stringify({ uid })
		});

		const text = await res.text();

		if(!res.ok) {
			showMessage(text || "Failed to mark attendance.", true);
			return;
		}

		showMessage("Attendance marked!");
		form.reset();
		input.focus();
	}
	catch {
		showMessage("Could not connect to the server.", true);
	}
	finally {
		button.disabled = false;
	}
});
