document.getElementById("solver").addEventListener("submit", async (event) => {
    event.preventDefault();

    const a = Number(document.getElementById("a").value);
    const b = Number(document.getElementById("b").value);
    const c = Number(document.getElementById("c").value);
    const method = document.getElementById("method").value;

    clearOutput();

    if (!Number.isFinite(a) || !Number.isFinite(b) || !Number.isFinite(c)) {
        showError("Please enter valid coefficients.");
        return;
    }

    if (a === 0) {
        showError("a cannot be 0 because this is not a quadratic equation.");
        return;
    }

    try {
        const response = await fetch("/M2/Math/quadratic-solver", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({
                a: a,
                b: b,
                c: c,
                method: method
            })
        });

        const data = await response.json();

        if (!response.ok && !data.steps) {
            showError(data.message || "Unable to solve the equation.");
            return;
        }

        await renderResult(data);
    }
    catch (error) {
        showError("Could not connect to the server.");
    }
});

function clearOutput() {
    document.getElementById("result").classList.add("hidden");
    document.getElementById("error").classList.add("hidden");

    document.getElementById("roots").innerHTML = "";
    document.getElementById("steps").innerHTML = "";

    const crossVisual = document.getElementById("cross-visual");
    crossVisual.innerHTML = "";
    crossVisual.classList.add("hidden");
}

async function renderResult(data) {
    const resultSection = document.getElementById("result");
    const methodName = document.getElementById("method-name");

    resultSection.classList.remove("hidden");
    methodName.textContent = data.methodName || "Solution";

    renderRoots(data);
    renderCrossVisual(data);
    renderSteps(data);

    if (data.message) {
        const message = document.createElement("div");
        message.className = "message";
        message.textContent = data.message;

        document.getElementById("steps").prepend(message);
    }

    await renderMathJax();
}

function renderRoots(data) {
    const roots = document.getElementById("roots");
    roots.innerHTML = "";

    if (data.hasRealRoots === false) {
        roots.innerHTML = `
            <div class="root-card">
                <span>No real roots</span>
            </div>
        `;

        return;
    }

    if (data.root1 === undefined || data.root2 === undefined) return;

    const root1 = Number(data.root1);
    const root2 = Number(data.root2);

    if (Math.abs(root1 - root2) < 1e-9) {
        roots.innerHTML = `
            <div class="root-card">
                <small>Root</small>
                <span>\\(x = ${formatNumber(root1)}\\)</span>
            </div>
        `;

        return;
    }

    roots.innerHTML = `
        <div class="roots-grid">
            <div class="root-card">
                <small>Root 1</small>
                <span>\\(x = ${formatNumber(root1)}\\)</span>
            </div>

            <div class="root-card">
                <small>Root 2</small>
                <span>\\(x = ${formatNumber(root2)}\\)</span>
            </div>
        </div>
    `;
}

function renderSteps(data) {
    const steps = document.getElementById("steps");
    steps.innerHTML = "";

    if (!Array.isArray(data.steps)) return;

    data.steps.forEach((step, index) => {
        const card = document.createElement("div");
        card.className = "step-card";

        const number = document.createElement("div");
        number.className = "step-number";
        number.textContent = index + 1;

        const content = document.createElement("div");
        content.className = "step-content";

        const title = document.createElement("h3");
        title.textContent = step.title;

        const expression = document.createElement("div");
        expression.className = "expression";
        expression.textContent = `\\[${step.latex}\\]`;

        content.appendChild(title);
        content.appendChild(expression);

        card.appendChild(number);
        card.appendChild(content);

        steps.appendChild(card);

        if (index !== data.steps.length - 1) {
            const arrow = document.createElement("div");
            arrow.className = "step-arrow";
            arrow.textContent = "↓";
            steps.appendChild(arrow);
        }
    });
}

function renderCrossVisual(data) {
    const container = document.getElementById("cross-visual");

    container.innerHTML = "";
    container.classList.add("hidden");

    if (!data.crossVisual?.enabled) return;

    const v = data.crossVisual;

    container.classList.remove("hidden");

    container.innerHTML = `
        <h3>Cross Method Layout</h3>

        <div class="cross-layout">
            <div class="cross-cell top-left">
                \\(${v.topLeft}\\)
            </div>

            <div class="cross-cell top-right">
                \\(${v.topRight}\\)
            </div>

            <div class="cross-cell top-cross">
                \\(${v.topCross}\\)
            </div>

            <div class="cross-cell bottom-left">
                \\(${v.bottomLeft}\\)
            </div>

            <div class="cross-cell bottom-right">
                \\(${v.bottomRight}\\)
            </div>

            <div class="cross-cell bottom-cross">
                \\(${v.bottomCross}\\)
            </div>

            <div class="cross-cell product-a">
                \\(${v.productA}\\)
            </div>

            <div class="cross-cell product-c">
                \\(${v.productC}\\)
            </div>

            <div class="cross-cell middle">
                \\(${v.middle}\\)
            </div>
        </div>
    `;
}

async function renderMathJax() {
    if (!window.MathJax) return;

    if (MathJax.typesetPromise) {
        await MathJax.typesetPromise();
    }
}

function showError(message) {
    const error = document.getElementById("error");

    error.textContent = message;
    error.classList.remove("hidden");
}

function formatNumber(value) {
    const number = Number(value);

    if (!Number.isFinite(number)) return "?";

    if (Math.abs(number - Math.round(number)) < 1e-9) {
        return String(Math.round(number));
    }

    return String(Number(number.toFixed(6)));
}