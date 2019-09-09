public class StaticValues extends StaticParent {
  public static GenericInterface<Foo> genericInterfaceSimpleMore =
      new GenericImplementationSimpleMore<Foo, Bar>(new Foo(2, 6), new Bar(11));
  public static void testGenericInterfaceSimpleMorePass() {
    assert genericInterfaceSimpleMore.size() == 2 && genericInterfaceSimpleMore.first().get() == 18
        && genericInterfaceSimpleMore.num() == 21;
  }
  public static void testGenericInterfaceSimpleMoreFail() {
    assert genericInterfaceSimpleMore.size() != 2 || genericInterfaceSimpleMore.first().get() != 18
        || genericInterfaceSimpleMore.num() != 21;
  }

  public static GenericInterface<Foo> genericInterfaceMore =
      new GenericImplementationMore<Foo, Bar>(new Foo[]{new Foo(1, 5)}, new Bar[]{new Bar(15), new Bar(12)});
  public static void testGenericInterfaceMorePass() {
    assert genericInterfaceMore.size() == 3 && genericInterfaceMore.first().get() == 16 &&
        genericInterfaceMore.num() == 25;
  }
  public static void testGenericInterfaceMoreFail() {
    assert genericInterfaceMore.size() != 3 || genericInterfaceMore.first().get() != 16 ||
        genericInterfaceMore.num() != 25;
  }

  public static GenericInterface<Foo> genericInterfaceLess =
      new GenericImplementationLess(new Foo[]{new Foo(1, 5)});
  public static void testGenericInterfaceLessPass() {
    assert genericInterfaceLess.size() == 1 && genericInterfaceLess.first().get() == 16 &&
        genericInterfaceLess.num() == 16;
  }
  public static void testGenericInterfaceLessFail() {
    assert genericInterfaceLess.size() != 1 || genericInterfaceLess.first().get() != 16 ||
        genericInterfaceLess.num() != 16;
  }
}
