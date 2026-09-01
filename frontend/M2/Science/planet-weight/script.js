document.getElementById("calculator").addEventListener("submit", async (event) => {
    event.preventDefault();

    const mass = Number(document.getElementById("mass").value);

    if (!Number.isFinite(mass) || mass < 0) {
        alert("Please enter a valid mass.");
        return;
    }

    const response = await fetch("/M2/Science/planetWeightCalculator", {
        method: "POST",
        headers: {
            "Content-Type": "application/json"
        },
        body: JSON.stringify({
            mass: mass
        })
    });

    if (!response.ok) {
        alert("Calculation failed.");
        return;
    }

    const data = await response.json();

    for (const planet in data) {
        const cell = document.getElementById(planet);

        if (cell) {
            cell.textContent = Number(data[planet]).toFixed(2) + " N";
        }
    }
});