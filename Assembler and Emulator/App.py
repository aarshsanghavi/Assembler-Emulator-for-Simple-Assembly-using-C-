import streamlit as st
import subprocess
import os
import tempfile

st.set_page_config(layout="wide")
st.title("🛠️ Simple Assembler + Emulator Interface")

ASSEMBLER_PATH = "Mainassembler.exe"
EMULATOR_PATH = "Mainemulator.exe"

# ──────────────── STEP 1 ────────────────
st.header("📄 Upload .asm file (for Assembly)")

uploaded_asm = st.file_uploader("Upload your `.asm` file", type=["asm"])

if uploaded_asm:
    with tempfile.TemporaryDirectory() as tmpdir:
        st.text(f"Temp dir path (Assembler): {tmpdir}")
        asm_path = os.path.join(tmpdir, uploaded_asm.name)
        with open(asm_path, "wb") as f:
            f.write(uploaded_asm.read())

        base_name = os.path.splitext(os.path.basename(asm_path))[0]

        if st.button("Assemble"):
            st.subheader("Assembler Output:")
            try:
                result = subprocess.run(
                    [ASSEMBLER_PATH, os.path.basename(asm_path)],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True,
                    cwd=tmpdir
                )

                st.code(result.stdout + result.stderr)

                log_path = os.path.join(tmpdir, base_name + ".log")
                if os.path.exists(log_path):
                    with open(log_path, encoding='utf-8', errors='ignore') as f:
                        st.text_area("Log (.log):", f.read(), height=150)

                lst_path = os.path.join(tmpdir, base_name + ".lst")
                if os.path.exists(lst_path):
                    with open(lst_path, encoding='utf-8', errors='ignore') as f:
                        st.text_area("Listing (.lst):", f.read(), height=250)

                o_path = os.path.join(tmpdir, base_name + ".o")
                if os.path.exists(o_path):
                    st.success(f"Object file created: {base_name}.o")
                    st.session_state['o_file_content'] = open(o_path, 'rb').read()
                    st.session_state['o_file_name'] = base_name + ".o"
                else:
                    st.error("Object file (.o) not created.")

            except Exception as e:
                st.error(f"Assembler failed: {e}")

# ──────────────── STEP 2 ────────────────
st.header("Upload or Use .o File for Emulator")

uploaded_obj = st.file_uploader("Upload your `.o` object file (optional)", type=["o"])
cmd = st.selectbox("Choose Emulator Command", [
    "-run", "-t", "-dump", "-reg", "-read", "-write", "-isa"
])

if uploaded_obj or 'o_file_content' in st.session_state:
    with tempfile.TemporaryDirectory() as tmpdir:
        if uploaded_obj:
            obj_path = os.path.join(tmpdir, uploaded_obj.name)
            with open(obj_path, "wb") as f:
                f.write(uploaded_obj.read())
        else:
            obj_path = os.path.join(tmpdir, st.session_state['o_file_name'])
            with open(obj_path, "wb") as f:
                f.write(st.session_state['o_file_content'])

        if st.button("Run Emulator"):
            try:
                result = subprocess.run(
                    [EMULATOR_PATH, obj_path, cmd],
                    stdout=subprocess.PIPE,
                    stderr=subprocess.PIPE,
                    text=True
                )

                full_output = result.stdout + result.stderr

                st.subheader("Raw Emulator Output:")
                st.code(full_output)

                if cmd == "-run":
                    init_dump = []
                    final_dump = []
                    regs = []
                    section = None

                    for line in full_output.splitlines():
                        line_upper = line.upper()
                        if "INITIAL MEMORY DUMP" in line_upper:
                            section = "initial"
                            continue
                        elif "FINAL MEMORY DUMP" in line_upper:
                            section = "final"
                            continue
                        elif "REGISTERS" in line_upper:
                            section = "regs"
                            continue
                        elif "EXECUTION HALTED" in line_upper:
                            section = None

                        if section == "initial":
                            init_dump.append(line)
                        elif section == "final":
                            final_dump.append(line)
                        elif section == "regs":
                            regs.append(line)

                    if init_dump:
                        st.text_area("Initial Memory", "\n".join(init_dump), height=150)
                    if final_dump:
                        st.text_area("Final Memory", "\n".join(final_dump), height=150)
                    if regs:
                        st.text_area("Registers", "\n".join(regs), height=100)

            except Exception as e:
                st.error(f"Emulator failed: {e}")
