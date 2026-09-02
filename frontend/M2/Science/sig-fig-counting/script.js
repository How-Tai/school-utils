document.getElementById("sig-fig-form").addEventListener("submit", async (event) => {
	event.preventDefault();

	const input = document.getElementById("input").value.trim();

	clearOutput();

	if(input.length === 0) {
		showError("Please enter a number.");
		return;
	}

	try {
		const response = await fetch("/M2/Science/sigFigCounter", {
			method: "POST",
			headers: {
				"Content-Type": "application/json"
			},
			body: JSON.stringify({
				input: input
			})
		});

		if(!response.ok) {
			let message = "Invalid number.";

			try {
				const data = await response.json();
				if(data.message) message = data.message;
			}
			catch {
				const text = await response.text();
				if(text) message = text;
			}

			showError(message);
			return;
		}

		const data = await response.json();

		document.getElementById("count").textContent = data.count;
		document.getElementById("result").classList.remove("hidden");
	}
	catch(error) {
		showError("Could not process the request.");
	}
});

function clearOutput() {
	document.getElementById("error").classList.add("hidden");
	document.getElementById("result").classList.add("hidden");
}

function showError(message) {
	const error = document.getElementById("error");

	error.textContent = message;
	error.classList.remove("hidden");
}