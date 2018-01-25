package biotools.biotoolsCompose.poc.sib;

import java.awt.Desktop;
import java.io.File;
import java.io.IOException;

import de.metaframe.jabc.framework.execution.ExecutionEnvironment;
import de.metaframe.jabc.framework.sib.parameter.ContextKey;
import de.metaframe.jabc.framework.sib.parameter.ContextKey.Scope;

public abstract class AbstractShowSIB extends AbstractSIB {

	public ContextKey file = new ContextKey("path", Scope.LOCAL, true);
	
	@Override
	public String trace(ExecutionEnvironment env) {
		try {
			File fileToShow = new File((String) env.get(file));
			Desktop.getDesktop().open(fileToShow);
			return "default";
		} catch (IOException e) {
			e.printStackTrace();
			return "error";
		}
	}

}
