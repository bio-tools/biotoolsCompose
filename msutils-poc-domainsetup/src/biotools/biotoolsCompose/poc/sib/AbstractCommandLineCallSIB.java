package biotools.biotoolsCompose.poc.sib;

import de.metaframe.jabc.framework.execution.ExecutionEnvironment;
import de.metaframe.jabc.framework.sib.parameter.ContextKey;
import de.metaframe.jabc.framework.sib.parameter.ContextKey.Scope;

public abstract class AbstractCommandLineCallSIB extends AbstractSIB {

	public String workingDirectory = "workingdir";
	public String executable = "executable";
	public String arguments = "arguments with placeholders for ${inFile} and ${outFile}"; 
	
	public ContextKey infile = new ContextKey("inFile", Scope.LOCAL, true);
	public ContextKey outfile = new ContextKey("outFile", Scope.LOCAL, true);
		
	@Override
	public String trace(ExecutionEnvironment env) {
		// TO DO execute the command
		//String executionresult = de.jabc.adapter.common.io.ServiceAdapter.executeCommand(env, executable, arguments, workingDirectory, exitCode, consoleOutput);
	
		
		// TO DO: "log" the executed command into a script file in the working dir
		// ...
		return null;
	}
	
}
