public class InnerClasses {
    public class PublicInnerClass {
        public int i;
        public PublicInnerClass(int i) {
            this.i = i;
        }
    }
    class DefaultInnerClass {
        int i;
        DefaultInnerClass(int i) {
            this.i = i;
        }
    }
    protected class ProtectedInnerClass {
        protected int i;
        protected ProtectedInnerClass(int i) {
            this.i = i;
        }
    }
    private class PrivateInnerClass {
        private int i;
        private PrivateInnerClass(int i) {
            this.i = i;
        }
    }
}