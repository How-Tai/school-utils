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

	const studentId = input.value.trim();

	if(!/^\d+$/.test(studentId)) {
		showMessage("Student ID must contain only numbers.", true);
		return;
	}

	button.disabled = true;
	message.hidden = true;

	try {
		const res = await fetch("/api/attendance", {
			method: "POST",
			headers: {
				"Content-Type": "application/json"
			},
			body: JSON.stringify({
				uid: studentId
			})
		});

		const data = await res.json();

		if(!res.ok) {
			showMessage(data.error || "Could not mark attendance.", true);
			return;
		}

		showMessage(data.message || "Attendance marked.");
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
