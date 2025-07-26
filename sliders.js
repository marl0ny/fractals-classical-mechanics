
function createScalarParameterSlider(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.for = spec['id']
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = ${spec.value}`
    controls.appendChild(label);
    let slider = document.createElement("input");
    slider.type = "range";
    slider.style ="width: 95%;"
    for (let k of Object.keys(spec))
        slider[k] = spec[k];
    slider.value = spec.value;
    controls.appendChild(document.createElement("br"));
    controls.appendChild(slider);
    controls.appendChild(document.createElement("br"));
    slider.addEventListener("input", e => {
        let valueF = Number.parseFloat(e.target.value);
        let valueI = Number.parseInt(e.target.value);
        if (type === "float") {
            label.textContent = `${sliderLabelName} = ${valueF}`
            Module.set_float_param(enumCode, valueF);
        } else if (type === "int") {
            label.textContent = `${sliderLabelName} = ${valueI}`
            Module.set_int_param(enumCode, valueI);
        }
    });
};

function createCheckbox(controls, enumCode, name, value) {
    let label = document.createElement("label");
    // label.for = spec['id']
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${name}`
    let checkbox = document.createElement("input");
    checkbox.type = "checkbox";
    // slider.style ="width: 95%;"
    // checkbox.value = value;
    checkbox.checked = value;
    // controls.appendChild(document.createElement("br"));
    controls.appendChild(checkbox);
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    checkbox.addEventListener("input", e => {
        console.log(e.target.checked);
        Module.set_bool_param(enumCode, e.target.checked);
    }
    );
}

let gVecParams = {};

function createVectorParameterSliders(
    controls, enumCode, sliderLabelName, type, spec) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = `${sliderLabelName} = (${spec.value})`
    gVecParams[sliderLabelName] = spec.value;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    for (let i = 0; i < spec.value.length; i++) {
        let slider = document.createElement("input");
        slider.type = "range";
        slider.style ="width: 95%;"
        for (let k of Object.keys(spec))
            slider[k] = spec[k][i];
        slider.value = spec.value[i];
        controls.appendChild(slider);
        controls.appendChild(document.createElement("br"));
        slider.addEventListener("input", e => {
            let valueF = Number.parseFloat(e.target.value);
            let valueI = Number.parseInt(e.target.value);
            if (type === "Vec2" || 
                type === "Vec3" || type === "Vec4") {
                gVecParams[sliderLabelName][i] = valueF;
                label.textContent 
                    = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`
                Module.set_vec_param(
                    enumCode, spec.value.length, i, valueF);
            } else if (type === "IVec2" || 
                        type === "IVec3" || type === "IVec4") {
                gVecParams[sliderLabelName][i] = valueI;
                label.textContent 
                    = `${sliderLabelName} = (${gVecParams[sliderLabelName]})`
                Module.set_ivec_param(
                    enumCode, spec.value.length, i, valueI);
            }
        });
    }
};

function createEntryBoxes(
    controls, enumCode, entryBoxName, count
) {
    let label = document.createElement("label");
    label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
    label.textContent = entryBoxName;
    controls.appendChild(label);
    controls.appendChild(document.createElement("br"));
    let entryBoxes = [];
    for (let i = 0; i < count; i++) {
        let entryBox = document.createElement('input');
        entryBox.type = "text";
        entryBox.value = "";
        entryBox.id = `entry-box-${enumCode}-${i}`;
        entryBox.style = "width: 95%;";
        let label = document.createElement("label");
        label.style = "color:white; font-family:Arial, Helvetica, sans-serif";
        label.textContent = `${i}`;
        controls.appendChild(label);
        controls.appendChild(document.createElement("br"));
        controls.appendChild(entryBox);
        controls.appendChild(document.createElement("br"));
        entryBoxes.push(entryBox);
        entryBox.addEventListener("input", e =>
            Module.set_string_param(enumCode, i, `${e.target.value}`)
        );
    }
}


let controls = document.getElementById('controls');
createScalarParameterSlider(controls, 0, "Steps/frame", "int", {'value': 10, 'min': 0, 'max': 100});
createScalarParameterSlider(controls, 1, "Time step (s)", "float", {'value': 0.001, 'min': -0.01, 'max': 0.01, 'step': 0.0001});
createScalarParameterSlider(controls, 2, "Mass 1 (kg)", "float", {'value': 1.0, 'min': 0.1, 'max': 10.0, 'step': 0.01});
createScalarParameterSlider(controls, 3, "Length 1 (m)", "float", {'value': 1.0, 'min': 0.1, 'max': 2.0, 'step': 0.01});
createScalarParameterSlider(controls, 4, "Mass 2 (kg)", "float", {'value': 1.0, 'min': 0.1, 'max': 10.0, 'step': 0.01});
createScalarParameterSlider(controls, 5, "Length 2 (m)", "float", {'value': 1.0, 'min': 0.1, 'max': 2.0, 'step': 0.01});
createScalarParameterSlider(controls, 6, "Acceleration due to gravity (m/s²)", "float", {'value': 9.81, 'min': 0.0, 'max': 20.0, 'step': 0.01});
createScalarParameterSlider(controls, 8, "Min. initial angle 1 (# of π radians)", "float", {'value': -1.0, 'min': -1.0, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 9, "Max. initial angle 1 (# of π radians)", "float", {'value': 1.0, 'min': -1.0, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 10, "Min. initial angle 2 (# of π radians)", "float", {'value': -1.0, 'min': -1.0, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 11, "Max. initial angle 2 (# of π radians)", "float", {'value': 1.0, 'min': -1.0, 'max': 1.0, 'step': 0.01});
createScalarParameterSlider(controls, 12, "Angle 1 discretization size", "int", {'value': 128, 'min': 32, 'max': 2048});
createScalarParameterSlider(controls, 13, "Angle 2 discretization size", "int", {'value': 128, 'min': 32, 'max': 2048});
createScalarParameterSlider(controls, 14, "Sub sample width", "int", {'value': 1, 'min': 1, 'max': 1024});
createScalarParameterSlider(controls, 15, "Sub sample height", "int", {'value': 1, 'min': 1, 'max': 1024});

