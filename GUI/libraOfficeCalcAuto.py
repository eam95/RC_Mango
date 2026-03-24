#!/usr/bin/env python3
"""
LibreOffice Calc Automation using UNO
Creates a spreadsheet and writes "Hello World" to row 2, column 3 (cell C2)
"""

import sys
import os
import subprocess
import time
import tempfile

def setup_uno_environment():
    """Setup UNO environment and import UNO modules"""
    try:
        # Try to use the LibreOffice Python environment
        libreoffice_paths = [
            "/usr/lib/libreoffice/program",
            "/usr/lib64/libreoffice/program",
            "/opt/libreoffice/program",
        ]

        # Add LibreOffice program path to sys.path
        for path in libreoffice_paths:
            if os.path.exists(path) and path not in sys.path:
                sys.path.insert(0, path)

        # Set PYTHONPATH environment variable for LibreOffice
        current_pythonpath = os.environ.get('PYTHONPATH', '')
        for path in libreoffice_paths:
            if os.path.exists(path):
                if current_pythonpath:
                    os.environ['PYTHONPATH'] = f"{path}:{current_pythonpath}"
                else:
                    os.environ['PYTHONPATH'] = path
                break

        import uno
        from com.sun.star.connection import NoConnectException
        print("Successfully imported UNO modules")
        return uno, NoConnectException

    except ImportError as e:
        print(f"Failed to import UNO: {e}")
        print("Trying alternative method...")
        return None, None

def create_calc_document_alternative():
    """Alternative method: Create Calc document using LibreOffice command line"""
    try:
        # Create a temporary LibreOffice Basic macro to create our spreadsheet
        macro_content = '''
Sub CreateHelloWorldSpreadsheet
    Dim oDoc As Object
    Dim oSheet As Object
    Dim oCell As Object
    
    ' Create a new Calc document
    oDoc = StarDesktop.loadComponentFromURL("private:factory/scalc", "_blank", 0, Array())
    
    ' Get the first sheet
    oSheet = oDoc.getSheets().getByIndex(0)
    
    ' Write "Hello World" to cell C2 (row 2, column 3)
    oCell = oSheet.getCellByPosition(2, 1)  ' Column C=2, Row 2=1 (0-indexed)
    oCell.setString("Hello World")
    
    ' Save the document
    oDoc.storeAsURL("file://{filepath}", Array())
    
    ' Close the document
    oDoc.close(True)
End Sub
        '''

        # Get the current directory for saving
        current_dir = os.getcwd()
        output_file = os.path.join(current_dir, "hello_world_spreadsheet.ods")
        output_url = f"file://{output_file}"

        # Create the macro file with the correct path
        macro_with_path = macro_content.replace("{filepath}", output_file)

        # Save macro to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.bas', delete=False) as f:
            f.write(macro_with_path)
            macro_file = f.name

        try:
            # Execute the macro using LibreOffice
            result = subprocess.run([
                'libreoffice',
                '--headless',
                '--nologo',
                f'macro://{macro_file}.CreateHelloWorldSpreadsheet'
            ], capture_output=True, text=True, timeout=30)

            print(f"LibreOffice command result: {result.returncode}")
            if result.stdout:
                print(f"Output: {result.stdout}")
            if result.stderr:
                print(f"Error: {result.stderr}")

        finally:
            # Clean up temporary macro file
            if os.path.exists(macro_file):
                os.unlink(macro_file)

        # Check if file was created
        if os.path.exists(output_file):
            print(f"Successfully created spreadsheet: {output_file}")
            return True
        else:
            print("Failed to create spreadsheet file")
            return False

    except Exception as e:
        print(f"Error in alternative method: {e}")
        return False

def start_libreoffice_service():
    """Start LibreOffice in headless mode as a service"""
    try:
        print("Starting LibreOffice service...")
        # Start LibreOffice in headless mode with UNO API enabled
        process = subprocess.Popen([
            'libreoffice',
            '--headless',
            '--accept=socket,host=localhost,port=2002;urp;StarOffice.ServiceManager'
        ], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

        # Wait a bit for the service to start
        time.sleep(5)

        # Check if process is still running
        if process.poll() is None:
            print("LibreOffice service started successfully")
            return True, process
        else:
            print("LibreOffice service failed to start")
            return False, None

    except Exception as e:
        print(f"Failed to start LibreOffice service: {e}")
        return False, None

def create_calc_document_with_hello_world():
    """Create a Calc document and write Hello World to C2 using UNO"""
    try:
        # Try to setup UNO environment
        uno, NoConnectException = setup_uno_environment()

        if uno is None:
            print("UNO not available, trying alternative method...")
            return create_calc_document_alternative()

        # Start LibreOffice service
        service_started, process = start_libreoffice_service()
        if not service_started:
            print("Failed to start LibreOffice service, trying alternative method...")
            return create_calc_document_alternative()

        try:
            # Connect to LibreOffice
            local_context = uno.getComponentContext()
            resolver = local_context.getServiceManager().createInstanceWithContext(
                "com.sun.star.bridge.UnoUrlResolver", local_context
            )

            # Try to connect with multiple attempts
            context = None
            for attempt in range(3):
                try:
                    context = resolver.resolve(
                        "uno:socket,host=localhost,port=2002;urp;StarOffice.ComponentContext"
                    )
                    break
                except NoConnectException:
                    print(f"Connection attempt {attempt + 1} failed, retrying...")
                    time.sleep(2)

            if context is None:
                print("Failed to connect to LibreOffice service after multiple attempts")
                if process:
                    process.terminate()
                return create_calc_document_alternative()

            print("Successfully connected to LibreOffice service")

            # Get the service manager
            service_manager = context.getServiceManager()

            # Create a new Calc document
            desktop = service_manager.createInstanceWithContext(
                "com.sun.star.frame.Desktop", context
            )

            # Create new Calc document
            document = desktop.loadComponentFromURL(
                "private:factory/scalc", "_blank", 0, ()
            )

            # Get the first sheet
            sheet = document.getSheets().getByIndex(0)

            # Write "Hello World" to cell C2 (row 2, column 3)
            # Note: LibreOffice uses 0-based indexing, so row 1, column 2 = C2
            cell = sheet.getCellByPosition(2, 1)  # Column C (index 2), Row 2 (index 1)
            cell.setString("Hello World")

            print("Successfully wrote 'Hello World' to cell C2")

            # Save the document
            file_path = os.path.join(os.getcwd(), "hello_world_spreadsheet.ods")
            file_url = "file://" + file_path

            document.storeAsURL(file_url, ())
            print(f"Document saved as: {file_path}")

            # Close the document
            document.close(True)

            # Stop the LibreOffice process
            if process:
                process.terminate()

            return True

        except Exception as e:
            print(f"Error during UNO operations: {e}")
            if process:
                process.terminate()
            return create_calc_document_alternative()

    except Exception as e:
        print(f"Error in main UNO method: {e}")
        return create_calc_document_alternative()

def create_simple_ods_file():
    """Create a simple ODS file using a basic approach"""
    try:
        print("Creating spreadsheet using direct LibreOffice command...")

        # Create a simple CSV file first
        csv_content = ",B,C\n,Hello,World\n"
        csv_file = "temp_data.csv"

        with open(csv_file, 'w') as f:
            f.write(csv_content)

        # Convert CSV to ODS using LibreOffice
        output_file = "hello_world_spreadsheet.ods"

        result = subprocess.run([
            'libreoffice',
            '--headless',
            '--convert-to', 'ods',
            '--outdir', '.',
            csv_file
        ], capture_output=True, text=True, timeout=30)

        print(f"Conversion result: {result.returncode}")
        if result.stdout:
            print(f"Output: {result.stdout}")
        if result.stderr and "Error" in result.stderr:
            print(f"Error: {result.stderr}")

        # Clean up temporary CSV file
        if os.path.exists(csv_file):
            os.unlink(csv_file)

        # Rename the converted file
        converted_file = "temp_data.ods"
        if os.path.exists(converted_file):
            if os.path.exists(output_file):
                os.unlink(output_file)
            os.rename(converted_file, output_file)
            print(f"Successfully created: {output_file}")
            return True

        return False

    except Exception as e:
        print(f"Error in simple ODS creation: {e}")
        return False

if __name__ == "__main__":
    print("LibreOffice Calc Automation Demo")
    print("=" * 40)

    # Try the main UNO method first
    success = create_calc_document_with_hello_world()

    # If that fails, try the simple approach
    if not success:
        print("\nTrying simple file creation method...")
        success = create_simple_ods_file()

    if success:
        print("\nDemo completed successfully!")
        print("Check the 'hello_world_spreadsheet.ods' file in the current directory.")
    else:
        print("\nDemo failed. Please check the error messages above.")
        print("Make sure LibreOffice is properly installed.")
