package biotools.biotoolsCompose.poc.sib;

import java.io.File;

import de.metaframe.jabc.framework.execution.ExecutionEnvironment;
import de.metaframe.jabc.framework.sib.parameter.ContextKey;
import de.metaframe.jabc.framework.sib.parameter.ContextKey.Scope;

public abstract class AbstractLoadSIB extends AbstractSIB {

	public File file = new File(System.getProperty("user.home"));
	public ContextKey contextKey = new ContextKey("nameForFile", Scope.LOCAL, true);
	
	@Override
	public String trace(ExecutionEnvironment env) {
		env.put(contextKey, file.getAbsolutePath());
		return "default";
	}
}
